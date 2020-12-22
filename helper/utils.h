#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>

using namespace std;

void yyerror(char *s, ...);
vector<pair<string, string> > destructDLDecl(string fields);
string constructDLDecl(vector<pair<string, string> > fieldPairs);
string constructDLRuleBegin(vector<string> fields);

#endif