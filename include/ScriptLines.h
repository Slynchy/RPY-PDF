#ifndef SCRLINES_H
#define SCRLINES_H

#include <vector>
#include <string>

static std::string FileTypeNames[2] = 
{
	".rpy",
	".utf"
};

enum FILETYPES
{
	RPY,				// Ren'Py
	UTF8,				// KEY
	NUM_OF_FILETYPES	// Unknown
};

struct ScriptFile
{
	std::wstring m_filepath;
	FILETYPES m_type;
};

class ScriptLine
{
	private:
	public:
		std::wstring m_line;
		int m_character;
		ScriptLine()
		{
			m_character = 0;
		};
		~ScriptLine()
		{
			m_line.clear();
			m_character = 0;
		};

};

class Chapter
{
	public:
		std::vector<ScriptLine> m_script;
		std::wstring m_name;
};

class Script
{
	private:
	public:
		std::vector<Chapter> m_chapter;
};

class ChapterName
{
	public:
		std::wstring m_codename;
		std::wstring m_name;
		std::wstring m_desc;
};

//Parses a wstring to string
static std::string to_string(std::wstring _input)
{
	std::string output;
	std::wstring input = _input;
	output.assign(input.begin(), input.end());
	return output;
};

//Parses a cstring to wstring
static std::wstring to_wstring(const char* _input)
{
	std::wstring output;
	std::string input = _input;
	output.assign(input.begin(), input.end());
	return output;
};

#endif