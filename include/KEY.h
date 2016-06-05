#ifndef KEY_H
#define KEY_H

#include "../ScriptLines.h"
#include "../DebugStrings.h"
#include <sstream>

int CleanScript_KEY(
	Script& input,
	std::vector<std::wstring>& _output, 
	std::vector<std::wstring> stringsToRemove,
	std::vector<std::wstring> stringsToReplace,
	std::vector<std::wstring> replacementStrings,
	std::vector<std::wstring> prefixesToRemove,
	std::vector<std::wstring> charNames,
	std::vector<std::wstring> charNames_fixed,
	std::vector<std::wstring> _KEY_FlagStrings);											// The main cleaning algorithm; takes a script and the debug string vector as parameters

#endif