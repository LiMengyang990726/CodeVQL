
#ifndef SYMBOLSTORE_H
#define SYMBOLSTORE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

using namespace std;

void initializeSymbolTable();
void QLObjToDLDecl(string key);
string QLObjToDLDeclWP(string key);
string QLObjToDLOutput(string key);

void storeVarDeclarationTable(string type, string name);
string findVarDeclarationWithoutPrint(string name);

void storeRuleReferenceTable(string type, string name);
string findRuleReferenceWithoutPrint(string name);

void storeOutputVar(string output);
string getOutputVar();
#endif
