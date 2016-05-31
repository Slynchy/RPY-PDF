#ifndef SCRLINES_H
#define SCRLINES_H

#include <vector>
#include <string>

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

#endif