
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

using namespace std;

void initializeSymbolTable();
void findSymbol(string key);
string findSymbolWithoutPrint(string key);

void storeVarDeclarationTable(string type, string name);
string findVarDeclarationWithoutPrint(string name);

void storeRuleReferenceTable(string type, string name);
string findRuleReferenceWithoutPrint(string name);

void storeOutputVar(string output);
string getOutputVar();
#endif
