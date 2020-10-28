
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

using namespace std;

void initialize();
void findSymbol(string key);
string findSymbolWithoutPrint(string key);
string findVarDeclSymbolWithoutPrint(string name);
string findReferenceSymbol(string name);
void storeVarSymbolTable(char* type, char* name);

#endif
