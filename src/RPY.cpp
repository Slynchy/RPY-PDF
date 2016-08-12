#include "RPY.h"

int CleanScript_RPY(
	Script& input,
	std::vector<std::wstring>& _output, 
	std::vector<std::wstring> stringsToRemove,
	std::vector<std::wstring> stringsToReplace,
	std::vector<std::wstring> replacementStrings,
	std::vector<std::wstring> prefixesToRemove,
	std::vector<std::wstring> charNames,
	std::vector<std::wstring> charNames_fixed,
	std::vector<std::wstring> debugStrings)
{
	for(size_t chap = 0; chap < input.m_chapter.size(); chap++)
	{

		Chapter& _input = input.m_chapter.at(chap);

		if(_input.m_name.substr(0, 3) == to_wstring("en_"))
		{
			_input.m_name.erase(0,3);
		};

		for(int l = 0; l < int(_input.m_script.size()); ++l)
		{

			RemoveCurlyParenthesis(_input.m_script.at(l).m_line);

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
						//if(d == 2)
						//{
						//	printf("I'm a breakpoint\n");
						//};
						debugStrings_funcs[d](_input.m_script.at(l).m_line, success);
						//TestLua(to_string(_input.m_script.at(l).m_line), l);
						if(success == true) breakout = true;
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
						_input.m_script.at(l).m_line.insert (0, charNames_fixed.at(c) + to_wstring(": "));
						success = true;
						break;
					};
				};
			};
			
			if(!success)
			{
				// Narrative or unknown character speech
				if(_input.m_script.at(l).m_line.at(0) == '\"')
				{
					if( std::count(_input.m_script.at(l).m_line.begin(), _input.m_script.at(l).m_line.end(), '\"') == 4 )
					{
						// 4 quotation marks = unknown character speech
						ScriptLine* ACTV_LINE = &_input.m_script.at(l);

						// So erase first quotation mark
						_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.begin());

						// find second one and erase
						size_t tempPos = _input.m_script.at(l).m_line.find_first_of('\"', 0);
						_input.m_script.at(l).m_line.erase(tempPos, 1);

						// Replace with colon
						_input.m_script.at(l).m_line.insert (tempPos, to_wstring(":"));
					}
					else 
					{
						_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.begin());
						_input.m_script.at(l).m_line.erase(_input.m_script.at(l).m_line.end() - 1);
					};
				}
				else 
				{
					_output.push_back(_input.m_script.at(l).m_line);
					_input.m_script.erase(_input.m_script.begin() + l);
					--l;
				};
			};
		};
	};
	return 0;
};