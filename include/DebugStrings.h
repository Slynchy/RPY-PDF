#ifndef DEBUGSTR_H
#define DEBUGSTR_H

#include <string>
#include <vector>

#include "../src/lua/lua.hpp"

void TestLua(std::string _temp, unsigned int _pos);

void AddSignLanguageBracketsHisao(std::wstring& _string, bool& _success);
void AddSignLanguageBracketsShizune(std::wstring& _string, bool& _success);
void WrittenNoteParser(std::wstring& _string, bool& _success);

void RemoveCurlyParenthesis(std::wstring& _string);

std::string to_string(std::wstring _input);
std::wstring to_wstring(const char* _input);
std::wstring to_wstring(std::string _input);

static void (* debugStrings_funcs[3])(std::wstring&, bool&) = { AddSignLanguageBracketsShizune, AddSignLanguageBracketsHisao, WrittenNoteParser };

#endif