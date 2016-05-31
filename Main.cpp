/*
	RPY-eBook
		By Sam Lynch

	For parsing .rpy script into eBook-friendly text
*/

#include "ScriptLines.h"
#include "DebugStrings.h"

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
static std::vector<std::wstring> scriptFiles;

static std::vector<std::wstring> stringsToReplace;
static std::vector<std::wstring> replacementStrings;

static std::vector<std::wstring> stringsToRemove;

static std::vector<std::wstring> prefixesToRemove;

static std::vector<std::wstring> charNames;
static std::vector<std::wstring> charNames_fixed;

static std::vector<std::vector<int>> g_chapterDividers; 

static std::vector<std::wstring> chapterNames;
static std::vector<ChapterName> chapterNames_parsed;

// Prototypes
int WriteScriptToTXT(std::vector<Script>& fileLines, const char* _filename);
int WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines, const char* _filename);
int LoadRequiredTextFiles(void);
int CleanScript(Script& _input, std::vector<std::wstring>&);
int LoadScriptIntoVector( const wchar_t* _input, Script& _vector );
int LoadTXTIntoVector( const char* _file, std::vector<std::wstring>& _vector );

int ParseChapters(std::vector<std::wstring>&, std::vector<ChapterName>&);

std::string to_string(std::wstring _input)
{
	std::string output;
	std::wstring input = _input;
	output.assign(input.begin(), input.end());
	return output;
};

std::wstring to_wstring(const char* _input)
{
	std::wstring output;
	std::string input = _input;
	output.assign(input.begin(), input.end());
	return output;
};

int main(int argc, char* argv[])
{
	std::cout << "RPY-eBook (c) Sam Lynch" << std::endl << std::endl;

	// Start the clock for benchmarking
	clock_t begin = clock();
	
	// Load all the configs into the appropriate vectors
	if(LoadRequiredTextFiles() != 0) return -1;

	//ParseChapters(chapterNames, chapterNames_parsed);
	
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
		wprintf( to_wstring("Loading: %s\n").c_str(), scriptFiles.at(i).c_str() );
		int error_code = 0;
		error_code = LoadScriptIntoVector( scriptFiles.at(i).c_str(), fileLines.at(i) );
		if(error_code != 0) return -2;
		
		// Cleaning
		wprintf( to_wstring("Cleaning script file: %s\n").c_str(), scriptFiles.at(i).c_str() );
		CleanScript(fileLines.at(i), fileLines_debug.at(i));
	};

	if( WriteScriptToTXT(fileLines, "output.txt") != 0 ) return -3;
	if( WriteScriptToTXT(fileLines_debug, "output_debug.txt") != 0 ) return -4;

	// Print time to complete and wait for input
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	elapsed_secs *= 1000;
	int elapsed_millisecs = (int)elapsed_secs;
	printf("\nFinished in %ims! Press any key to exit...\n", elapsed_millisecs);
	_getch();

	fileLines.clear();
	fileLines_debug.clear();
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

	// Add new entry to chapter dividers
	std::vector<int> temp;
	g_chapterDividers.push_back(temp);

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
			_vector.m_chapter.back().m_script.push_back(temp);
		};
	};
	inputFile.seekg (0, inputFile.beg);

	// Cleanup
	inputFile.close();
	return 0;
};

int CleanScript(Script& input, std::vector<std::wstring>& _output)
{
	for(size_t chap = 0; chap < input.m_chapter.size(); chap++)
	{

		Chapter& _input = input.m_chapter.at(chap);

		/*char response = 0;
		bool foundChapterName = false;

		if(_input.m_name.substr(0, 3) == to_wstring("en_"))
		{
			_input.m_name.erase(0,3);
		};

		for(size_t n = 0; n < chapterNames_parsed.size(); n++)
		{
			if(_input.m_name == chapterNames_parsed.at(n).m_codename)
			{
				std::wcout << "\nWould you like to keep the following chapter? Y/N" << '\n' << '	' << chapterNames_parsed.at(n).m_name << '\n' << '	' << chapterNames_parsed.at(n).m_desc << '\n';
				foundChapterName = true;
				break;
			};
		};

		if(!foundChapterName)
		{
			printf("\n\nWould you like to keep chapter: %s? Y/N", to_string(_input.m_name).c_str());
		};

		while(response != 'Y' && response != 'y' && response != 'N' && response != 'n')
		{
			response = _getch();
		};

		if(response == 'Y' || response == 'y')
		{*/

			for(int l = 0; l < int(_input.m_script.size()); ++l)
			{
				size_t temp = 0;
				for(int c = 0; c < (int)stringsToRemove.size(); ++c)
				{
					size_t temp = 0;
					while(temp != std::wstring::npos)
					{
						temp = _input.m_script.at(l).m_line.find(stringsToRemove.at(c), 0);
						if(temp != std::wstring::npos)
						{
							_input.m_script.at(l).m_line.erase(
								temp,
								stringsToRemove.at(c).length());
						} 
						else 
						{
							break;
						};
					};
				};
				for(int c= 0; c < (int)stringsToReplace.size(); ++c)
				{
					size_t temp = 0;
					while(temp != std::wstring::npos)
					{
						temp = _input.m_script.at(l).m_line.find(stringsToReplace.at(c), 0);
						if(temp != std::wstring::npos)
						{
							_input.m_script.at(l).m_line.erase(
								temp,
								stringsToReplace.at(c).length());
							_input.m_script.at(l).m_line.insert(temp, replacementStrings.at(c));
						} 
						else 
						{
							break;
						};
					};
				};
		
				bool success = false;
				bool breakout = false;
				for(size_t d = 0; d < debugStrings.size(); d++)
				{
					size_t temp = 0;
					while(temp != std::wstring::npos)
					{
						temp = _input.m_script.at(l).m_line.find(debugStrings.at(d), 0);
				
						if(temp != std::string::npos)
						{
							debugStrings_funcs[d](_input.m_script.at(l).m_line, success);
							breakout = true;
							break;
						}
						else
						{
							break;
						};
					};
					if(breakout) break;
				};

				std::wstringstream iss(_input.m_script.at(l).m_line); 
				std::wstring item;
				iss >> item;

				std::string chapterdivide_str = "label";
				std::wstring chapterdivide_wstr;
				chapterdivide_wstr.assign(chapterdivide_str.begin(), chapterdivide_str.end());

				if(item == chapterdivide_wstr && !success)
				{
					g_chapterDividers.back().push_back(l);
				};

				if(!success){
					for(int c = 0; c < (int)prefixesToRemove.size(); ++c)
					{
						if(item == prefixesToRemove.at(c))
						{
							_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.begin(), _input.m_script.at(l).m_line.begin() + (prefixesToRemove.at(c).length() + 1));
							break;
						};
					};
				};

				if(!success){
					for(int c = 0; c < (int)charNames.size(); ++c)
					{
						if(item == charNames.at(c))
						{
							_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.begin(), _input.m_script.at(l).m_line.begin() + (charNames.at(c).length() + 1));
							std::string colon(": ");
							std::wstring wcolon;
							wcolon.assign(colon.begin(), colon.end());
							_input.m_script.at(l).m_line.insert (0, charNames_fixed.at(c) + wcolon);
							success = true;
							break;
						};
					};
				};
			
				if(!success)
				{
					if(_input.m_script.at(l).m_line.at(0) == '\"')
					{
						_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.begin());
						_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.end() - 1);
					}
					else 
					{
						_output.push_back(_input.m_script.at(l).m_line);
						_input.m_script.erase(_input.m_script.begin() + l);
						--l;
					};
				};
			};
	/*	}
		else 
		{
			_input.m_script.clear();
		};
	};*/
	};
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
	error_code = LoadTXTIntoVector("scripts/scriptfiles.txt", scriptFiles);
	
	//printf("Loading: %s\n", "scripts/chapternames.txt");
	//error_code = LoadTXTIntoVector("scripts/chapternames.txt", chapterNames);

	return error_code;
};

int WriteScriptToTXT(std::vector<Script>& fileLines, const char* _filename = "output.txt")
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
				if(fileLines.at(f).m_chapter.at(chap).m_script.size() != 0) textOutput << "-----------------------------------------------\n\n";
			};
			textOutput << "###################################################\n###################################################\n\n";
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
				/*for(size_t ch = 0; ch < g_chapterDividers.at(f).size(); ch++)
				{
					if( (l) == g_chapterDividers.at(f).at(ch) )
					{
						textOutput << "-----------------------------------------------\n\n";
						break;
					};
				};*/
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
	//std::wstring str = to_wstring("(\"Out Cold\", \"NOP1\", \"On a cold, snowy day, Hisao's dreams were about to be realized, only to be cut short by a sudden heart attack.\", (\"Act 1\",\"Prologue\")),");

	// Remove spaces
	/*str.erase(std::remove_if(str.begin(), str.end(), isspace));
	str.erase(std::remove_if(str.begin(), str.end(), isspace));*/

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