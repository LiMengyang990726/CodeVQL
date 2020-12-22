#ifndef PRINTUTILS_H
#define PRINTUTILS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>

using namespace std;

void printTemplate();
void printDecl(string type);
void printOutputDecl(string type);
void printInput(string type);
void printOutput();
void printRuleBegin();
void printRuleTermination();
void printRule(string name, string field, string value);

#endif