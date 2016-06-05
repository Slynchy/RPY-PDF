#include "KEY.h"

int CleanScript_KEY(
	Script& input,
	std::vector<std::wstring>& _output, 
	std::vector<std::wstring> stringsToRemove,
	std::vector<std::wstring> stringsToReplace,
	std::vector<std::wstring> replacementStrings,
	std::vector<std::wstring> prefixesToRemove,
	std::vector<std::wstring> charNames,
	std::vector<std::wstring> charNames_fixed,
	std::vector<std::wstring> _KEY_FlagStrings)
{
	Chapter& _input = input.m_chapter.at(0);

	std::vector<std::wstring> CharacterNames;

	for(int l = 0; l < int(_input.m_script.size()); ++l)
	{
		bool success = false;

		// Get character names of script
		if(_input.m_script.at(l).m_line.at(0) == '#')
		{
			std::wstring temp_name;
			size_t firstSign = _input.m_script.at(l).m_line.find_first_of('\'');
			size_t lastSign = _input.m_script.at(l).m_line.find_last_of('\'');
			if(firstSign != std::wstring::npos && lastSign != std::wstring::npos)
			{
				temp_name = _input.m_script.at(l).m_line.substr(firstSign+1,(lastSign-firstSign)-1);
				CharacterNames.push_back(temp_name);
			};
		};

		// Remove comments
		size_t commentLocation;
		commentLocation = _input.m_script.at(l).m_line.find(to_wstring("//"), 0);
		if(commentLocation != std::wstring::npos)
		{
			_input.m_script.at(l).m_line.erase(commentLocation, _input.m_script.at(l).m_line.size() - commentLocation);
			if(commentLocation == 0)
			{
				_output.push_back(_input.m_script.at(l).m_line);
				_input.m_script.erase(_input.m_script.begin() + l);
				--l;
				success = true;
			};
		};

		// Remove all extraneous lines
		if(!success)
		{
			if(_input.m_script.at(l).m_line.at(0) == '<')
			{
				// dialogue found
			} 
			else 
			{
				_output.push_back(_input.m_script.at(l).m_line);
				_input.m_script.erase(_input.m_script.begin() + l);
				--l;
				success = true;
			};
		};

		// Remove line number
		if(!success)
		{
			size_t lastSign = _input.m_script.at(l).m_line.find_first_of('>');
			if( lastSign != std::wstring::npos )
			{
				_input.m_script.at(l).m_line.erase(0, lastSign + 2 );
			};
		};

		// Remove debug flags
		if(!success)
		{
			for(size_t flag = 0; flag < _KEY_FlagStrings.size(); flag++)
			{
				size_t strLoc = 0;
				while(strLoc != std::wstring::npos)
				{
					std::wstring temp = to_wstring("\\")+_KEY_FlagStrings.at(flag);
					strLoc = _input.m_script.at(l).m_line.find(temp, 0);
					if(strLoc != std::wstring::npos)
					{
						size_t closeParenthesisLoc = _input.m_script.at(l).m_line.find_first_of('}', strLoc);
						_input.m_script.at(l).m_line.erase(strLoc, closeParenthesisLoc - strLoc+1);
					};
				};
			};
		};

		if(!success)
		{
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
		};
		
		if(!success)
		{
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
		};

		// Get + format character name
		if(!success)
		{
			if(_input.m_script.at(l).m_line.size() > 0)
			{
				if(_input.m_script.at(l).m_line.at(0) == '\\')
				{
					std::wstring temp_name;
					size_t firstSign = _input.m_script.at(l).m_line.find_first_of('{');
					size_t lastSign = _input.m_script.at(l).m_line.find_first_of('}');
					if(firstSign != std::wstring::npos && lastSign != std::wstring::npos)
					{
						temp_name = _input.m_script.at(l).m_line.substr(firstSign+1,(lastSign-firstSign)-1);
						_input.m_script.at(l).m_line.erase(0,lastSign+1);

						for(size_t name = 0; name < CharacterNames.size(); name++)
						{
							if(CharacterNames.at(name) == temp_name)
							{
								_input.m_script.at(l).m_line.insert(0, CharacterNames.at(name) + to_wstring(":"));
							};
						};

						//_input.m_script.at(l).m_line.insert(0, temp_name + to_wstring(":"));
					};
				};
			}
			else 
			{
				_input.m_script.erase(_input.m_script.begin() + l);
				--l;
				success = true;
			};
		};
		
		// Remove Dangopedia references
		if(!success)
		{
			if( _input.m_script.at(l).m_line.find(to_wstring("}={"), 0) != std::wstring::npos)
			{
				std::wstring tempStr;
				size_t firstParPos;
				size_t secondParPos;
				size_t lastParPos;
				firstParPos = _input.m_script.at(l).m_line.find_first_of('{', 0);
				secondParPos = _input.m_script.at(l).m_line.find_first_of('}', 0);
				lastParPos = _input.m_script.at(l).m_line.find_last_of('}');
				tempStr = _input.m_script.at(l).m_line.substr( firstParPos + 1, secondParPos - firstParPos - 1 );

				_input.m_script.at(l).m_line.erase(firstParPos, lastParPos - firstParPos + 1);
				_input.m_script.at(l).m_line.insert(firstParPos, tempStr);
			};
		};
	};


	return 0;
};