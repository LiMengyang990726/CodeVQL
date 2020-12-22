
#ifndef SYMBOLSTORE_H
#define SYMBOLSTORE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

using namespace std;

void initializeSymbolTable();
string QLObjToDLDecl(string key);
string QLObjToDLOutput(string key);

void storeVarDeclarationTable(string type, string name);
string findVarDeclaration(string name);

void storeRuleReferenceTable(string type, string name);
string findRuleReference(string name);

void storeOutputVar(string output);
string getOutputVar();
#endif
