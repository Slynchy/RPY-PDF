#include "DebugStrings.h"

void TestLua(std::string _temp, unsigned int _pos)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lua_pushstring( L, _temp.c_str() );
	lua_setglobal( L, "_INPUT_STR" );

	std::string temp	=	"scripts/debugstrings/";
	temp				+=	_pos;
	temp				+=	".lua";

	luaL_dofile(L, temp.c_str());
	lua_close(L);
};

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

std::wstring to_wstring(std::string _input)
{
	std::wstring output;
	output.assign(_input.begin(), _input.end());
	return output;
};

void WrittenNoteParser(std::wstring& _string, bool& _success)
{
	int quoteMark1 = _string.find_first_of('"');
	int quoteMark2 = _string.find_last_of('"');
	
	_string.erase(quoteMark2+1, _string.length() - (quoteMark2+1));
	_string.erase(0, quoteMark1);
	_success = true;

	return;
};

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

void AddSignLanguageBracketsHisao(std::wstring& _string, bool& _success)
{
	std::string test = "his";
	std::wstring testws;
	testws.assign(test.begin(), test.end());
	if(_string.find(testws) != 0)
	{
		_success = false;
		return;
	};

	test = "Hisao: ";
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

void AddSignLanguageBracketsShizune(std::wstring& _string, bool& _success)
{
	std::string test = "ssh";
	std::wstring testws;
	testws.assign(test.begin(), test.end());
	if(_string.find(testws) != 0)
	{
		_success = false;
		return;
	};

	test = "Shizune: ";
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