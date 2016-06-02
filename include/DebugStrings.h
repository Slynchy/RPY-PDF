#ifndef DEBUGSTR_H
#define DEBUGSTR_H

#include <string>
#include <vector>

static std::vector<std::wstring> debugStrings;

void IamAdebugStringFunction(std::wstring& _string, bool& _success);

void RemoveCurlyParenthesis(std::wstring& _string);

static void (* debugStrings_funcs[2])(std::wstring&, bool&) = { IamAdebugStringFunction };

#endif