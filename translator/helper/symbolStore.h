
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
int getFieldLen(string type);
string CodeVQLObjToSouffleDecl(string type);
string CodeVQLObjToSouffleOutput(unordered_map<string, string> outputVarFieldTable);
string CodeVQLObjToSouffleRuleBegin(unordered_map<string, string> outputVarFieldTable);
bool isDirectFieldExtraction(string name, string field);
bool isTypeValidClosure(string type);
bool isClosureMethod(string field);
bool isDiffType(string type);

void storeValidVersionForDiffTypeSet(string versionName, vector<string> rangeStrs);
bool isValidVersionForDiffTypeSet(string versionName);
void addTypeToDiffTypeVersionTypeAssocTable(string versionName, string type);
void writeDiffTypeVersionTypeAssoc();

void storeVarDeclarationTable(string type, string name);
string findVarDeclaration(string name);

void storeVersionDeclarationTable(string name);
bool isVersionDeclared(string name);
vector<string> getVersions();

void storeVersionVarAssociationTable(string version, string name);
string findVersionVarAssociation(string name);

void storeVarFieldReferenceTable(string referred, string referer);
string findVarFieldReferredName(string name, string field);

void storeOutputVarField(string output, string field);
unordered_map<string, string> getOutputVarFieldTable();

void storenotExistSpecifiedVarsSet(string name);
void clearnotExistSpecifiedVarsSet();
bool isVariableSpecifiedInNotExist(string name);

#endif
