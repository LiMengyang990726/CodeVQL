
#ifndef SYMBOLSTORE_H
#define SYMBOLSTORE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

using namespace std;

void initializeSymbolTable();
string getIdName(string type);
string CodeVQLObjToSouffleDecl(string type);
string CodeVQLObjToSouffleOutput(unordered_set<string> outputVarsSet);
string CodeVQLObjToSouffleRuleBegin(unordered_set<string> outputVarsSet);
bool isDirectFieldExtraction(string name, string field);
bool isClosureMethod(string name, string field);

void storeVarDeclarationTable(string type, string name);
string findVarDeclaration(string name);

void storeVersionDeclarationTable(string name);
bool isVersionDeclared(string name);
vector<string> getVersions();

void storeVersionVarAssociationTable(string version, string name);
string findVersionVarAssociation(string name);

void storeVarFieldReferenceTable(string referred, string referer);
string findVarFieldReferredName(string name, string field);

void storeOutputVar(string output);
unordered_set<string> getoutputVarsSet();

void storenotExistSpecifiedVarsSet(string name);
void clearnotExistSpecifiedVarsSet();
bool isVariableSpecifiedInNotExist(string name);

#endif
