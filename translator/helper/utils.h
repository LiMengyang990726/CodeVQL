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
vector<pair<string, string> > destructSouffleDecl(string fields);
string constructSouffleDecl(vector<pair<string, string> > fieldPairs);
string constructSouffleRuleBegin(vector<string> fields);
bool isNumber(string s);

#endif