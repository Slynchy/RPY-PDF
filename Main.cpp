/*
	RPY-PDF
		By Sam Lynch

	For parsing rpy script into PDF format
*/

#include "PDF.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include "conio.h"

static std::vector<std::string> scriptFiles;

static std::vector<std::string> stringsToReplace;
static std::vector<std::string> replacementStrings;

static std::vector<std::string> stringsToRemove;

static std::vector<std::string> debugPrefixes;

static std::vector<std::string> charNames;
static std::vector<std::string> charNames_fixed;

int LoadTXTIntoVector( const char* _file, std::vector<std::string>& _vector )
{
	std::ifstream inputFile;
	inputFile.open(_file);
	
	if(!inputFile){ 
		printf("Failed to load: %s\n", _file);
		return -1;
	};
	
	std::string line;
	while (std::getline(inputFile , line)) 
	{
		_vector.push_back(line);
	};
	
	inputFile.close();
	return 0;
};

int LoadScriptIntoVector( const char* _input, std::vector<std::string>& _vector )
{
	// Load file
	std::ifstream inputFile;
	inputFile.open(_input);

	// Check file
	if(!inputFile){ 
		printf("Failed to load: %s\n", _input);
		return -1;
	};

	// Load meaningful lines into vector
	std::size_t lines_count = 0;
	std::string line;
	while (std::getline(inputFile , line)) 
	{
		if(line != ""){
			++lines_count;
			_vector.push_back(line);
		};
	};
	inputFile.seekg (0, inputFile.beg);

	// Cleanup
	inputFile.close();
	return 0;
};

int CleanScript(std::vector<std::string>& _input)
{
	for(int l = 0; l < int(_input.size()); ++l)
	{
		size_t temp = 0;
		for(int c = 0; c < (int)stringsToRemove.size(); ++c)
		{
			size_t temp = 0;
			while(temp != std::string::npos)
			{
				temp = _input.at(l).find(stringsToRemove.at(c), 0);
				if(temp != std::string::npos)
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
			while(temp != std::string::npos)
			{
				temp = _input.at(l).find(stringsToReplace.at(c), 0);
				if(temp != std::string::npos)
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
		std::stringstream iss(_input.at(l)); 
		std::string item;
		bool success = false;
		iss >> item;
		for(int c = 0; c < (int)debugPrefixes.size(); ++c)
		{
			if(item == debugPrefixes.at(c))
			{
				_input.at(l).erase(_input.at(l).begin(), _input.at(l).begin() + (debugPrefixes.at(c).length() + 1));
				_input.at(l).erase(_input.at(l).end() - 1);
				_input.at(l).erase(size_t(0), size_t(1));
				success = true;
				break;
			};
		};
		if(!success){
			for(int c = 0; c < (int)charNames.size(); ++c)
			{
				if(item == charNames.at(c))
				{
					_input.at(l).erase(_input.at(l).begin(), _input.at(l).begin() + (charNames.at(c).length() + 1));
					_input.at(l).insert (0, charNames_fixed.at(c) + ": ");
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

int main(int argc, char **argv)
{
	clock_t begin = clock();

	printf("Loading: %s\n", "scripts/charnames.txt");
	LoadTXTIntoVector("scripts/charnames.txt", charNames);

	printf("Loading: %s\n", "scripts/charnames_fixed.txt");
	LoadTXTIntoVector("scripts/charnames_fixed.txt", charNames_fixed);
	
	printf("Loading: %s\n", "scripts/debugprefixes.txt");
	LoadTXTIntoVector("scripts/debugprefixes.txt", debugPrefixes);
	
	printf("Loading: %s\n", "scripts/replacementstrings.txt");
	LoadTXTIntoVector("scripts/replacementstrings.txt", replacementStrings);
	
	printf("Loading: %s\n", "scripts/stringstoreplace.txt");
	LoadTXTIntoVector("scripts/stringstoreplace.txt", stringsToReplace);
	
	printf("Loading: %s\n", "scripts/stringstoremove.txt");
	LoadTXTIntoVector("scripts/stringstoremove.txt", stringsToRemove);
	
	printf("Loading: %s\n", "scripts/scriptfiles.txt");
	LoadTXTIntoVector("scripts/scriptfiles.txt", scriptFiles);


	std::vector<std::vector<std::string>> fileLines;
	fileLines.resize(scriptFiles.size());

	for(size_t i = 0; i < scriptFiles.size(); i++)
	{
		printf("Loading: %s\n", scriptFiles.at(i).c_str());
		LoadScriptIntoVector(scriptFiles.at(i).c_str(), fileLines.at(i));
		printf("Cleaning script file: %s\n", scriptFiles.at(i).c_str());
		CleanScript(fileLines.at(i));
	};
	
	HPDF_Doc pdf = HPDF_New (NULL, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return -1;
    };
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);
	const char* font_name = HPDF_LoadTTFontFromFile (pdf, "playtime.ttf", HPDF_TRUE);
    std::vector<HPDF_Page> pages;
	printf("Writing to PDF...\n");
	WriteScriptToPDF(fileLines, pdf, font_name, pages);
	HPDF_SaveToFile(pdf, "anus.pdf");
	HPDF_Free (pdf);
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	elapsed_secs *= 1000;
	printf("\nFinished in %f ms! Press any key to exit...\n", elapsed_secs);
	_getch();

	fileLines.clear();
	return 0;
};