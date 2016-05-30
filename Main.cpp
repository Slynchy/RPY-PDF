/*
	RPY-eBook
		By Sam Lynch

	For parsing rpy script into eBook-friendly format
*/

#include "PDF.h"
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

std::vector<std::vector<int>> g_chapterDividers; 

// Prototypes
void WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines);
void LoadRequiredTextFiles(void);
int CleanScript(std::vector<std::wstring>& _input);
int LoadScriptIntoVector( const wchar_t* _input, std::vector<std::wstring>& _vector );
int LoadTXTIntoVector( const char* _file, std::vector<std::wstring>& _vector );
void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);

int main(int argc, char **argv)
{
	// Start the clock for benchmarking
	clock_t begin = clock();
	
	// Start the clock for benchmarking
	LoadRequiredTextFiles();

	// Create wstring versions of constants
	std::wstring loadingstr_w;
	std::wstring cleaningstr_w;
	loadingstr_w.assign(loadingstr.begin(), loadingstr.end());
	cleaningstr_w.assign(cleaningstr.begin(), cleaningstr.end());
	
	// Create and resize the primary script file vector to the number of scripts
	std::vector<std::vector<std::wstring>> fileLines;
	fileLines.resize(scriptFiles.size());

	// Loop through all the scripts, loading and cleaning them
	for(size_t i = 0; i < scriptFiles.size(); i++)
	{
		// Loading
		wprintf(				loadingstr_w.c_str(), 
								scriptFiles.at(i).c_str()
								);
		LoadScriptIntoVector(	scriptFiles.at(i).c_str(), 
								fileLines.at(i)
								);
		
		// Cleaning
		wprintf(
								cleaningstr_w.c_str(), 
								scriptFiles.at(i).c_str()
								);
		CleanScript(fileLines.at(i));
	};

	
	// Initialize PDF doc
	HPDF_Doc pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return -1;
    };
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);
    std::vector<HPDF_Page> pages;

	// Initialize font file
	const char* font_name = HPDF_LoadTTFontFromFile (pdf, "font.ttf", HPDF_TRUE);

	printf("Writing to PDF...\n");
	WriteScriptToPDF(fileLines, pdf, font_name, pages,g_chapterDividers);
	HPDF_SaveToFile(pdf, "output.pdf");
	HPDF_Free (pdf);

	WriteScriptToTXT(fileLines);

	// Print time to complete and wait for input
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	elapsed_secs *= 1000;
	printf("\nFinished in %f ms! Press any key to exit...\n", elapsed_secs);
	_getch();

	fileLines.clear();
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

int CleanScript(std::vector<std::wstring>& _input)
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

		if(!success){
			for(int c = 0; c < (int)prefixesToRemove.size(); ++c)
			{
				if(item == prefixesToRemove.at(c))
				{
					_input.at(l).erase(_input.at(l).begin(), _input.at(l).begin() + (prefixesToRemove.at(c).length() + 1));
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
				_input.erase(_input.begin() + l);
				--l;
			};
		};
	};
	return 0;
};

void LoadRequiredTextFiles(void)
{
	printf("Loading: %s\n", "scripts/charnames.txt");
	LoadTXTIntoVector("scripts/charnames.txt", charNames);

	printf("Loading: %s\n", "scripts/charnames_fixed.txt");
	LoadTXTIntoVector("scripts/charnames_fixed.txt", charNames_fixed);
	
	printf("Loading: %s\n", "scripts/debugstrings.txt");
	LoadTXTIntoVector("scripts/debugstrings.txt", debugStrings);
	
	printf("Loading: %s\n", "scripts/replacementstrings.txt");
	LoadTXTIntoVector("scripts/replacementstrings.txt", replacementStrings);
	
	printf("Loading: %s\n", "scripts/stringstoreplace.txt");
	LoadTXTIntoVector("scripts/stringstoreplace.txt", stringsToReplace);
	
	printf("Loading: %s\n", "scripts/stringstoremove.txt");
	LoadTXTIntoVector("scripts/stringstoremove.txt", stringsToRemove);

	printf("Loading: %s\n", "scripts/prefixestoremove.txt");
	LoadTXTIntoVector("scripts/prefixestoremove.txt", prefixesToRemove);
	
	printf("Loading: %s\n", "scripts/scriptfiles.txt");
	LoadTXTIntoVector("scripts/scriptfiles.txt", scriptFiles);
};

void WriteScriptToTXT(std::vector<std::vector<std::wstring>>& fileLines)
{
	printf("Writing to TXT...\n");
	std::ofstream textOutput("output.txt");
	for(size_t f = 0; f < fileLines.size(); ++f)
	{
		for(size_t l = 0; l < fileLines.at(f).size(); ++l)
		{
			std::string temp_str;
			temp_str.assign(fileLines.at(f).at(l).begin(), fileLines.at(f).at(l).end());
			textOutput.write(temp_str.c_str(), fileLines.at(f).at(l).size());
			textOutput << "\n\n";
		};
		textOutput << "################################################### \n\n";
	};
	textOutput.close();
};

void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%d\n",
      (unsigned int) error_no, (int) detail_no);
};