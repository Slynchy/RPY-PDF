#include "DebugStrings.h"

void RemoveCurlyParenthesis(std::wstring& _string)
{

	while(_string.find('{') != std::string::npos)
	{
		unsigned int first = _string.find('{');
		unsigned int last = _string.find_last_of('}');
		_string.erase (first, (last+1)-first);
	};

	return;
};

void IamAdebugStringFunction(std::wstring& _string, bool& _success)
{
	std::string test = "Name";
	std::wstring testws;
	testws.assign(test.begin(), test.end());
	if(_string.find(testws) != 0)
	{
		_success = false;
		return;
	};

	test = "Name: ";
	testws.clear();
	testws.assign(test.begin(), test.end());

	_string.erase(0, 5);
	_string.erase(_string.end() - 1);
	_string.insert(_string.begin(), '[');
	_string.insert(_string.end(),']');
	_string.insert(0, testws);
	_success = true;

	return;
};