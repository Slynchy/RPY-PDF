/*
	RPY-eBook
		By Sam Lynch

	For parsing .rpy script into eBook-friendly text
*/

#include "DebugStrings.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include "conio.h"
#include <codecvt>

// Constants
static const std::string loadingstr = "Loading: %s\n";
static const std::string cleaningstr = "Cleaning script file: %s\n";

// Arrays for storing text data
static std::vector<std::wstring> scriptFiles;

static std::vector<std::wstring> stringsToReplace;
static std::vector<std::wstring> replacementStrings;

static std::vector<std::wstring> stringsToRemove;

static std::vector<std::wstring> prefixesToRemove;

static std::vector<std::wstring> charNames;
static std::vector<std::wstring> charNames_fixed;

static std::vector<std::vector<int>> g_chapterDividers; 

// Prototypes
int WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines, const char* _filename);
int LoadRequiredTextFiles(void);
int CleanScript(std::vector<std::wstring>& _input, std::vector<std::wstring>&);
int LoadScriptIntoVector( const wchar_t* _input, std::vector<std::wstring>& _vector );
int LoadTXTIntoVector( const char* _file, std::vector<std::wstring>& _vector );

int main(int argc, char* argv[])
{
	std::cout << "RPY-eBook (c) Sam Lynch" << std::endl << std::endl;

	// Start the clock for benchmarking
	clock_t begin = clock();
	
	// Load all the configs into the appropriate vectors
	if(LoadRequiredTextFiles() != 0) return -1;

	// Create wstring versions of constants
	std::wstring loadingstr_w;
	std::wstring cleaningstr_w;
	loadingstr_w.assign(loadingstr.begin(), loadingstr.end());
	cleaningstr_w.assign(cleaningstr.begin(), cleaningstr.end());
	
	// Create and resize the primary script file vector to the number of scripts
	std::vector<std::vector<std::wstring>> fileLines;
	fileLines.resize(scriptFiles.size());

	// as well as the debug output (lines that are removed)
	std::vector<std::vector<std::wstring>> fileLines_debug;
	fileLines_debug.resize(scriptFiles.size());

	// Loop through all the scripts, loading and cleaning them
	for(size_t i = 0; i < scriptFiles.size(); i++)
	{
		// Loading
		wprintf( loadingstr_w.c_str(), scriptFiles.at(i).c_str() );
		int error_code = 0;
		error_code = LoadScriptIntoVector( scriptFiles.at(i).c_str(), fileLines.at(i) );
		if(error_code != 0) return -2;
		
		// Cleaning
		wprintf( cleaningstr_w.c_str(), scriptFiles.at(i).c_str() );
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

int LoadScriptIntoVector( const wchar_t* _input, std::vector<std::wstring>& _vector )
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
	std::size_t lines_count = 0;
	std::wstring line;
	std::string test = "";
	std::wstring testws;
	testws.assign(test.begin(), test.end());
	while (std::getline(inputFile , line)) 
	{
		if(line != testws){
			++lines_count;
			_vector.push_back(line);
		};
	};
	inputFile.seekg (0, inputFile.beg);

	// Cleanup
	inputFile.close();
	return 0;
};

int CleanScript(std::vector<std::wstring>& _input,std::vector<std::wstring>& _output)
{
	for(int l = 0; l < int(_input.size()); ++l)
	{
		size_t temp = 0;
		for(int c = 0; c < (int)stringsToRemove.size(); ++c)
		{
			size_t temp = 0;
			while(temp != std::wstring::npos)
			{
				temp = _input.at(l).find(stringsToRemove.at(c), 0);
				if(temp != std::wstring::npos)
				{
					_input.at(l).erase(
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
				temp = _input.at(l).find(stringsToReplace.at(c), 0);
				if(temp != std::wstring::npos)
				{
					_input.at(l).erase(
						temp,
						stringsToReplace.at(c).length());
					_input.at(l).insert(temp, replacementStrings.at(c));
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
				temp = _input.at(l).find(debugStrings.at(d), 0);
				
				if(temp != std::string::npos)
				{
					debugStrings_funcs[d](_input.at(l), success);
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

		std::wstringstream iss(_input.at(l)); 
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
					_input.at(l).erase(_input.at(l).begin(), _input.at(l).begin() + (prefixesToRemove.at(c).length() + 1));
					break;
				};
			};
		};

		if(!success){
			for(int c = 0; c < (int)charNames.size(); ++c)
			{
				if(item == charNames.at(c))
				{
					_input.at(l).erase(_input.at(l).begin(), _input.at(l).begin() + (charNames.at(c).length() + 1));
					std::string colon(": ");
					std::wstring wcolon;
					wcolon.assign(colon.begin(), colon.end());
					_input.at(l).insert (0, charNames_fixed.at(c) + wcolon);
					success = true;
					break;
				};
			};
		};
			
		if(!success)
		{
			if(_input.at(l).at(0) == '\"')
			{
				_input.at(l).erase(_input.at(l).begin());
				_input.at(l).erase(_input.at(l).end() - 1);
			}
			else 
			{
				_output.push_back(_input.at(l));
				_input.erase(_input.begin() + l);
				--l;
			};
		};
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

	return error_code;
};

int WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines, const char* _filename = "output.txt")
{
	printf("Writing to %s...\n", _filename);
	std::ofstream textOutput(_filename);

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
				for(size_t ch = 0; ch < g_chapterDividers.at(f).size(); ch++)
				{
					if( (l) == g_chapterDividers.at(f).at(ch) )
					{
						textOutput << "-----------------------------------------------\n\n";
						break;
					};
				};
				std::string temp_str;
				temp_str.assign(fileLines.at(f).at(l).begin(), fileLines.at(f).at(l).end());
				textOutput.write(temp_str.c_str(), fileLines.at(f).at(l).size());
				textOutput << "\n\n";
			};
			textOutput << "###################################################\n###################################################\n\n";
		};
		textOutput.close();
	};
	return 0;
};