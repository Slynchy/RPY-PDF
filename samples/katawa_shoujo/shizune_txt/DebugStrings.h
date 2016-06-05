#ifndef DEBUGSTR_H
#define DEBUGSTR_H

#include <string>
#include <vector>

void AddSignLanguageBracketsHisao(std::wstring& _string, bool& _success);
void AddSignLanguageBracketsShizune(std::wstring& _string, bool& _success);

void RemoveCurlyParenthesis(std::wstring& _string);

static void (* debugStrings_funcs[2])(std::wstring&, bool&) = { AddSignLanguageBracketsShizune, AddSignLanguageBracketsHisao };

#endif