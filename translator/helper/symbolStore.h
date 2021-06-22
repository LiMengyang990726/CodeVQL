
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
string EvoMeObjToSouffleDecl(string type);
string EvoMeObjToSouffleOutput(unordered_map<string, string> outputVarFieldTable);
string EvoMeObjToSouffleRuleBegin(unordered_map<string, string> outputVarFieldTable);
bool isDirectFieldExtraction(string name, string field);
bool isTypeValidClosure(string type);
bool isClosureMethod(string field);
bool isDiffType(string type);

void storeValidVersionForDiffTypeSet(string versionName, vector<string> rangeStrs);
bool isValidVersionForDiffTypeSet(string versionName);
void addTypeToDiffTypeVersionTypeAssocTable(string versionName, string type);
void writeDiffTypeVersionTypeAssoc();

void storeVarDeclarationTable(string type, const string& name);
string findVarDeclaration(const string& name);

void storeVersionDeclarationTable(const string& name);
bool isVersionDeclared(const string& name);
vector<string> getVersions();

void storeVersionVarAssociationTable(string version, const string& name);
string findVersionVarAssociation(const string& name);

void storeVarFieldReferenceTable(string referred, string referer);
string findVarFieldReferredName(const string& name, const string& field);

void storeOutputVarField(const string& output, string field);
void storeOutputAlias(const string& output, const string& field, string alias);
unordered_map<string, string> getOutputVarFieldTable();
void writeResultTableHeader();

void storenotExistSpecifiedVarsSet(const string& name);
void clearnotExistSpecifiedVarsSet();
bool isVariableSpecifiedInNotExist(const string& name);

#endif
