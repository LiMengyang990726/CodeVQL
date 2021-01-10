
#ifndef SYMBOLSTORE_H
#define SYMBOLSTORE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

using namespace std;

void initializeSymbolTable();
string QLObjToDLDecl(string key);
string QLObjToDLOutput(unordered_set<string> outputVars);
string QLObjToDLRuleBegin(unordered_set<string> outputVars);

void storeVarDeclarationTable(string type, string name);
string findVarDeclaration(string name);

void storeVersionDeclarationTable(string name);
bool findVersionDeclaration(string name);
unordered_set<string> getVersions();

void storeVersionVarAssociationTable(string version, string name);
string findVersionVarAssociation(string name);

void storeRuleReferenceTable(string reference, string name);
string findRuleReference(string name);

void storeOutputVar(string output);
unordered_set<string> getOutputVars();

#endif
