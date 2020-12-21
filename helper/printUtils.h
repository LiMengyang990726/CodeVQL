#ifndef PRINTUTILS_H
#define PRINTUTILS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>

using namespace std;

char getNextSymbol();
void printFormulaLHS();
void printRule(char* name, char* field, char* value);
char printRuleReturnReference(char* name, char* field);

#endif