/*
	RPY-eBook
		By Sam Lynch

	For parsing .rpy script into eBook-friendly text
*/

#include "ScriptLines.h"
#include "DebugStrings.h"

#include "KEY.h"
#include "RPY.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
#include "conio.h"
#include <codecvt>

// Arrays for storing text data
static std::vector<ScriptFile> scriptFiles;

static std::vector<std::wstring> stringsToReplace;
static std::vector<std::wstring> replacementStrings;

static std::vector<std::wstring> stringsToRemove;

static std::vector<std::wstring> prefixesToRemove;

static std::vector<std::wstring> charNames;
static std::vector<std::wstring> charNames_fixed;

static std::vector<std::wstring> chapterNames;
static std::vector<ChapterName> chapterNames_parsed;

static std::vector<std::wstring> KEY_FlagStrings;

static std::vector<std::wstring> debugStrings;

// Prototypes
int WriteScriptToTXT(std::vector<Script>& fileLines, const char* _filename);							// Writes the entire collection of scripts to TXT

int WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines, const char* _filename);			// Writes the array of array of strings to TXT (for the debug file)

int LoadRequiredTextFiles(void);																		// Simply runs LoadTXTIntoVector onto all the necessary files

int LoadScriptIntoVector( const wchar_t* _input, Script& _vector );										// Loads the specified file name into the specified script file

int LoadTXTIntoVector( const char* _file, std::vector<std::wstring>& _vector );							// Loads the specified file name into the specified vector of wstrings
int LoadScriptFileNamesIntoVector( const char* _file, std::vector<ScriptFile>& _vector );				// Same as above

int ParseChapters(std::vector<std::wstring>&, std::vector<ChapterName>&);								// Parses the array of wstrings and organises them into a vector of ChapterNames

int main(int argc, char* argv[])
{
	std::cout << "RPY-eBook (c) Sam Lynch" << std::endl << std::endl;

	// Start the clock for benchmarking
	clock_t begin = clock();
	
	// Load all the configs into the appropriate vectors
	if(LoadRequiredTextFiles() != 0)
	{
		_getch();
		return -1;
	};
	
	// Parse the chapternames into the array
	ParseChapters(chapterNames, chapterNames_parsed);
	
	// Create and resize the primary script file vector to the number of scripts
	std::vector<Script> fileLines;
	fileLines.resize(scriptFiles.size());

	// as well as the debug output (lines that are removed)
	std::vector<std::vector<std::wstring>> fileLines_debug;
	fileLines_debug.resize(scriptFiles.size());

	// Loop through all the scripts, loading and cleaning them
	for(size_t i = 0; i < scriptFiles.size(); i++)
	{
		// Loading
		wprintf( to_wstring("Loading: %s\n").c_str(), scriptFiles.at(i).m_filepath.c_str() );
		int error_code = 0;
		error_code = LoadScriptIntoVector( scriptFiles.at(i).m_filepath.c_str(), fileLines.at(i) );
		if(error_code != 0)
		{
			_getch();
			return -2;
		};
		
		// Cleaning
		wprintf( to_wstring("Cleaning script file: %s\n").c_str(), scriptFiles.at(i).m_filepath.c_str() );
		switch(scriptFiles.at(i).m_type)
		{
			case FILETYPES(RPY):
				CleanScript_RPY(fileLines.at(i), fileLines_debug.at(i), stringsToRemove, stringsToReplace, replacementStrings, prefixesToRemove, charNames, charNames_fixed, debugStrings);
				break;
			case FILETYPES(UTF8):
				CleanScript_KEY(fileLines.at(i), fileLines_debug.at(i), stringsToRemove, stringsToReplace, replacementStrings, prefixesToRemove, charNames, charNames_fixed, KEY_FlagStrings);
				break;
			default:
				break;
		};
	};

	if( WriteScriptToTXT(fileLines, "output.txt") != 0 ) return -3;
	if( WriteScriptToTXT(fileLines_debug, "output_debug.txt") != 0 ) return -4;

	// Print time to complete and wait for input
	clock_t end = clock();
	double elapsed_secs = double(end - begin);
	int elapsed_millisecs = (int)elapsed_secs;
	printf("\nFinished in %ims! Press any key to exit...\n", elapsed_millisecs);
	_getch();

	fileLines.clear();
	fileLines_debug.clear();
	return 0;
};

int LoadScriptFileNamesIntoVector( const char* _file, std::vector<ScriptFile>& _vector )
{
	std::wifstream inputFile;
	inputFile.open(_file);
	
	if(!inputFile){ 
		printf("Failed to load: %s\n", _file);
		return -1;
	};
	
	ScriptFile temp;
	while (std::getline(inputFile , temp.m_filepath)) 
	{
		size_t dotpos;
		dotpos = temp.m_filepath.find_last_of('.');
		if(dotpos != std::wstring::npos)
		{
			std::wstring FileTypeStr;
			int dbg2 = (dotpos - temp.m_filepath.size()) * -1;
			FileTypeStr = temp.m_filepath.substr( dotpos, dbg2);
			for(int n = 0; n < FILETYPES(NUM_OF_FILETYPES); n++)
			{
				if(FileTypeStr == to_wstring(FileTypeNames[n].c_str()))
				{
					temp.m_type = FILETYPES(n);
					break;
				};
			};
		};
		_vector.push_back(temp);
	};
	
	inputFile.close();
	return 0;
};

int LoadTXTIntoVector( const char* _file, std::vector<std::wstring>& _vector )
{
	std::wifstream inputFile;
	inputFile.open(_file);
	
	if(!inputFile){ 
		printf("Failed to load: %s\n", _file);
		return -1;
	};
	
	std::wstring line;
	while (std::getline(inputFile , line)) 
	{
		_vector.push_back(line);
	};
	
	inputFile.close();
	return 0;
};

int LoadScriptIntoVector( const wchar_t* _input, Script& _vector )
{
	// Load file
	std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>) ;
	std::wifstream inputFile;
	inputFile.open(_input);
	inputFile.imbue(ulocale);

	// Check file
	if(!inputFile){ 
		printf("Failed to load: %s\n", _input);
		return -1;
	};

	// Load meaningful lines into vector
	std::wstring line;
	while (std::getline(inputFile , line)) 
	{
		if(line != to_wstring("")){
			std::wstringstream iss(line); 
			std::wstring item;
			iss >> item;

			if(item == to_wstring("label")) {
				Chapter temp;
				iss >> temp.m_name;
				temp.m_name.erase(temp.m_name.end() - 1);
				_vector.m_chapter.push_back(temp);
			};

			ScriptLine temp;
			temp.m_line = line;
			if(_vector.m_chapter.size() == 0) 
			{
				Chapter temp;
				_vector.m_chapter.push_back(temp);
			};
			_vector.m_chapter.back().m_script.push_back(temp);
		};
	};
	inputFile.seekg (0, inputFile.beg);

	// Cleanup
	inputFile.close();
	return 0;
};

int LoadRequiredTextFiles(void)
{
	int error_code = 0;

	printf("Loading: %s\n", "scripts/charnames.txt");
	error_code = LoadTXTIntoVector("scripts/charnames.txt", charNames);

	printf("Loading: %s\n", "scripts/charnames_fixed.txt");
	error_code = LoadTXTIntoVector("scripts/charnames_fixed.txt", charNames_fixed);
	
	printf("Loading: %s\n", "scripts/debugstrings.txt");
	error_code = LoadTXTIntoVector("scripts/debugstrings.txt", debugStrings);
	
	printf("Loading: %s\n", "scripts/replacementstrings.txt");
	error_code = LoadTXTIntoVector("scripts/replacementstrings.txt", replacementStrings);
	
	printf("Loading: %s\n", "scripts/stringstoreplace.txt");
	error_code = LoadTXTIntoVector("scripts/stringstoreplace.txt", stringsToReplace);
	
	printf("Loading: %s\n", "scripts/stringstoremove.txt");
	error_code = LoadTXTIntoVector("scripts/stringstoremove.txt", stringsToRemove);

	printf("Loading: %s\n", "scripts/prefixestoremove.txt");
	error_code = LoadTXTIntoVector("scripts/prefixestoremove.txt", prefixesToRemove);
	
	printf("Loading: %s\n", "scripts/scriptfiles.txt");
	error_code = LoadScriptFileNamesIntoVector("scripts/scriptfiles.txt", scriptFiles);
	
	printf("Loading: %s\n", "scripts/chapternames.txt");
	error_code = LoadTXTIntoVector("scripts/chapternames.txt", chapterNames);
	
	printf("Loading: %s\n", "scripts/key_flagstrings.txt");
	error_code = LoadTXTIntoVector("scripts/key_flagstrings.txt", KEY_FlagStrings);

	return error_code;
};

int WriteScriptToTXT(std::vector<Script>& fileLines, const char* _filename = "output.txt")
{
	printf("Writing to %s...\n", _filename);
	std::wofstream textOutput(_filename);

	int chapterCount = 0;

	if(!textOutput)
	{
		printf("Failed to write %s!", _filename);
		return -1;
	}
	else
	{
		// Unicode support
		textOutput.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t,0x10ffff,std::generate_header>));

		//scripts/acts/files
		for(size_t f = 0; f < fileLines.size(); ++f)
		{
			for(size_t chap = 0; chap < fileLines.at(f).m_chapter.size(); ++chap)
			{
				//lines
				for(size_t l = 0; l < fileLines.at(f).m_chapter.at(chap).m_script.size(); ++l)
				{
					textOutput << fileLines.at(f).m_chapter.at(chap).m_script.at(l).m_line;
					textOutput << "\n\n";
				};

				if(fileLines.at(f).m_chapter.at(chap).m_script.size() != 0 && !( (f+1) == fileLines.size() && (chap+1) == fileLines.at(f).m_chapter.size() )) 
				{
					textOutput << "-----------------------------------------------\n";
					
					for(size_t n = 0; n < chapterNames_parsed.size(); n++)
					{
						std::wstring tempstr;
						int temp = chap;
						if(temp < (int)fileLines.at(f).m_chapter.size()-1)
						{
							temp++;
							tempstr = fileLines.at(f).m_chapter.at(temp).m_name;
						} 
						else 
						{
							if((f+1) < fileLines.size())
							{
								tempstr = fileLines.at(f+1).m_chapter.at(0).m_name;
							} 
							else 
							{
								tempstr = fileLines.back().m_chapter.at(0).m_name;
							};
						};
						if(tempstr == chapterNames_parsed.at(n).m_codename)
						{
							textOutput << "Chapter " << (++chapterCount) << " - " << chapterNames_parsed.at(n).m_name << "\n\n"; //<< '	' << chapterNames_parsed.at(n).m_desc << '\n';
							break;
						};
					};

				};
			};
			textOutput << "###################################\n\n";
		};
		textOutput.close();
	};
	return 0;
};

int WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines, const char* _filename = "output.txt")
{
	printf("Writing to %s...\n", _filename);
	std::wofstream textOutput(_filename);

	if(!textOutput)
	{
		printf("Failed to write %s!", _filename);
		return -1;
	}
	else
	{
		//scripts/acts/files
		for(size_t f = 0; f < fileLines.size(); ++f)
		{
			//lines
			for(size_t l = 0; l < fileLines.at(f).size(); ++l)
			{
				textOutput << fileLines.at(f).at(l);
				textOutput << "\n\n";
			};
			textOutput << "###################################################\n###################################################\n\n";
		};
		textOutput.close();
	};
	return 0;
};

int ParseChapters(std::vector<std::wstring>& _input, std::vector<ChapterName>& _output)
{
	printf("Parsing chapter names...\n");

	for(size_t n = 0; n < _input.size(); n++)
	{
		std::wstring& str = _input.at(n);

		// Remove initial brackets
		unsigned int first = str.find('(');
		unsigned int last = str.find_last_of(')');
		str = str.substr (first,last-first);

		std::vector<std::wstring> params;

		while(str.find('\"') != std::string::npos)
		{
			std::wstring temp;
			last = str.find_first_of('\"');
			temp = str.erase (0,last+1);
			last = str.find_first_of('\"');
			temp = str.substr (0,last);
			params.push_back(temp);
			str.erase(0, last+1);
		};

		if(params.size() >= 3)
		{
			ChapterName temp;
			temp.m_name = params.at(0);
			temp.m_codename = params.at(1);
			temp.m_desc = params.at(2);
			_output.push_back(temp);
		};
	};
	return 0;
};