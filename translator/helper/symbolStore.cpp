
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolStore.h"
#include "populateMainSouffle.h"
#include "utils.h"
#include "constants.h"
using namespace std;

struct diffTypeVersion {
   string startCommit;
   string endCommit;
};

unordered_map<string, string> symbolTable;
unordered_set<string> diffTypeSymbolSet;
unordered_set<string> validVersionForDiffTypeSet;
unordered_map<string, diffTypeVersion> diffTypeVersionNameDetailTable;
unordered_map<string, vector<string> > diffTypeVersionTypeAssocTable;
unordered_map<string, string> varDeclarationTable;
vector<string> versionDeclarationVec;
unordered_map<string, string> versionVarAssocTable;
unordered_map<string, unordered_map<string, string> > varFeildReferenceTable;
unordered_map<string, string> outputVarFieldTable;
unordered_set<string> notExistSpecifiedVarsSet;

/* APIs for Symbol Table */
void initializeSymbolTable() {
   symbolTable["TestClass"] = "(fqn: String, version: Version)";
   symbolTable["AbstractClass"] = "(fqn: String, version: Version)";
   symbolTable["Class"] = "(fqn: String, isAbstract: String, getSuperClass: String, isNested: String, getOuterClass: String, isTestClass: String, version: Version)";
   symbolTable["Method"] = "(fqn: String, getName: String, getClassName: String, getNumberOfParams: number, getReturn: String, isConstructor: String, version: Version)";

   symbolTable["Update"] = "(fqn: String, pos: String, version: Version, parent: Version)"; 
   diffTypeSymbolSet.insert("Update");
   symbolTable["Insert"] = "(fqn: String, pos: String, version: Version, parent: Version)";
   diffTypeSymbolSet.insert("Insert");
   symbolTable["Delete"] = "(fqn: String, pos: String, version: Version, parent: Version)";
   diffTypeSymbolSet.insert("Delete");
   
   symbolTable["MethodAccess"] = "(getCaller: String, getCallee: String, version: Version)";
   symbolTable["Containment"] = "(getContainer: String, getContainee: String, version: Version)";
   symbolTable["Reference"] = "(getReferer: String, getReferee: String, version: Version)";
   symbolTable["Inheritance"] = "(getInheritater: String, getInheritatee: String, version: Version)";
   
   symbolTable["History"] = "(child: Version, parent: Version, index: Int)";
   symbolTable["Version"] = "(version: Version)";
   return;
}

string getIdName(string type) {
   vector<pair<string, string> > fieldPairs = destructSouffleDecl(symbolTable[type]);
   if (fieldPairs.size() == 0 || fieldPairs[0].second != "String") {
      yyerror("There is something wrong during the translation process: type does not have id field");
   }
   return fieldPairs[0].first;
}

int getFieldLen(string type) {
   return destructSouffleDecl(symbolTable[type]).size();
}

string CodeVQLObjToSouffleDecl(string type) {
   string result = "";
   if (symbolTable.find(type) != symbolTable.end()) {
      result = symbolTable[type];
   } 
   return result;
}

string CodeVQLObjToSouffleOutput(unordered_map<string, string> outputVarFieldTable) {
   vector<pair<string, string> > resultPairs;
   for (auto iter = outputVarFieldTable.begin(); iter != outputVarFieldTable.end(); iter++) {
      string name = iter->first;
      string field = iter->second;
      string type = findVarDeclaration(name);
      if (symbolTable.find(type) != symbolTable.end()) {
         vector<pair<string, string> > fieldPairs = destructSouffleDecl(symbolTable[type]);
         for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
            string currField = it->first;
            if (currField == field) {
               resultPairs.push_back(make_pair(findVarFieldReferredName(name, field), it->second));
            } else if (currField == "version") {
               resultPairs.push_back(make_pair(currField, it->second));
            }
         }
      }
   }
   return constructSouffleDecl(resultPairs);
}

string CodeVQLObjToSouffleRuleBegin(unordered_map<string, string> outputVarFieldTable) {
   vector<string> resultPairs;
   for (auto iter = outputVarFieldTable.begin(); iter != outputVarFieldTable.end(); iter++) {
      string name = iter->first;
      string field = iter->second;
      string type = findVarDeclaration(name);
      if (symbolTable.find(type) != symbolTable.end()) {
         vector<pair<string, string> > fieldPairs = destructSouffleDecl(symbolTable[type]);
         for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
            string currField = it->first;
            if (currField == field) {
               resultPairs.push_back(findVarFieldReferredName(name, field));
            } else if (currField == "version") {
               resultPairs.push_back(findVersionVarAssociation(name));
            }
         }
      }
   }
   return constructSouffleRuleBegin(resultPairs);
}

bool isDirectFieldExtraction(string name, string field) {
   string type = findVarDeclaration(name);
   string fields = CodeVQLObjToSouffleDecl(type);
   vector<pair<string, string> > fieldsVector = destructSouffleDecl(fields);
   for(auto iter = fieldsVector.begin(); iter != fieldsVector.end(); iter++) {
      if(iter->first == field) {
         return true;
      }
   }
   return false;
}

bool isTypeValidClosure(string type) {
    vector<pair<string, string> > fieldPairs = destructSouffleDecl(symbolTable[type]);
    if (fieldPairs.size() < 3) {
        return false;
    }
    string firstFieldName = fieldPairs[0].first;
    string secondFieldName = fieldPairs[1].first;
    string lastFieldName = fieldPairs[fieldPairs.size()-1].first;
    if (!hasEnding(firstFieldName, CLOSURE_FIRST_FIELD_SUFFIX)) {
        return false;
    }
    if (!hasEnding(secondFieldName, CLOSURE_SECOND_FIELD_SUFFIX)) {
        return false;
    }
    if (!hasEnding(lastFieldName, CLOSURE_LAST_FIELD)) {
        return false;
    }
    return true;
}

bool isClosureMethod(string field) {
   return field.back() == CLOSURE_SUFFIX;
}

bool isDiffType(string type) {
   return diffTypeSymbolSet.find(type) != diffTypeSymbolSet.end();
}

/* APIs for Valid Versions (for Diff Type) Set*/
bool isValidVersionForDiffType(vector<string> rangeStrs) {
   if (rangeStrs.size() != 2) {
      return false;
   }
   if (!regex_match(rangeStrs[0], regex(PLAIN_VERSION_REGEX))) {
      return false;
   }
   if (!regex_match(rangeStrs[1], regex(PLAIN_VERSION_REGEX))) {
      return false;
   }
   return true;
}

void storeValidVersionForDiffTypeSet(string versionName, vector<string> rangeStrs) {
   if (isValidVersionForDiffType(rangeStrs)) {
      validVersionForDiffTypeSet.insert(versionName);
      diffTypeVersion d = {/* startCommit = */rangeStrs[1], /* endCommit = */rangeStrs[0] };
      diffTypeVersionNameDetailTable[versionName] = d;
   }
}

bool isValidVersionForDiffTypeSet(string versionName) {
   return validVersionForDiffTypeSet.find(versionName) != validVersionForDiffTypeSet.end();
}

void addTypeToDiffTypeVersionTypeAssocTable(string versionName, string type) {
   if (diffTypeVersionNameDetailTable.find(versionName) != diffTypeVersionNameDetailTable.end()) {
      diffTypeVersionTypeAssocTable[versionName].push_back(type);
   } else {
      return;
   }
}

void writeDiffTypeVersionTypeAssoc() {
   ofstream interMsgFile;
   interMsgFile.open(INTER_MSG_FOR_PIPELINE_FILE_NAME_ABS);

   for (auto it = diffTypeVersionTypeAssocTable.begin(); it != diffTypeVersionTypeAssocTable.end(); it++) {
      if (it->second.size() == 0) {
         continue;
      }
      diffTypeVersion d = diffTypeVersionNameDetailTable[it->first];
      string startCommit = d.startCommit;
      startCommit.erase(remove(startCommit.begin(), startCommit.end(), '\"' ), startCommit.end());
      string endCommit = d.endCommit;
      endCommit.erase(remove(endCommit.begin(), endCommit.end(), '\"' ), endCommit.end());
      vector<string> types = it->second;
      interMsgFile << startCommit << "\t" << endCommit << "\t";
      for (string type : types) {
         transform(type.begin(), type.end(),type.begin(), ::toupper); // All DIFF facts's names are in upper case
         interMsgFile << type << "\t";
      }
      interMsgFile << endl;
   }
   interMsgFile.close();
}

/* APIs for Var Declaration Table */
void storeVarDeclarationTable(string type, string name) {
   varDeclarationTable[name] = type;
}

string findVarDeclaration(string name) {
   string result = "";
   if (varDeclarationTable.find(name) != varDeclarationTable.end()) {
      result = varDeclarationTable[name];
   } 
   return result;
}

/* APIs for Version Declaration Table */
void storeVersionDeclarationTable(string name) {
   versionDeclarationVec.push_back(name);
}

bool isVersionDeclared(string name) {
   for (int i = 0; i < versionDeclarationVec.size(); i++) {
      if (versionDeclarationVec[i] == name) {
         return true;
      }
   }
   return false;
}

vector<string> getVersions() {
   return versionDeclarationVec;
}

/* APIs for Version Variable Association Table */
void storeVersionVarAssociationTable(string version, string name) {
   versionVarAssocTable[name] = version;
}

string findVersionVarAssociation(string name) {
   string result = "";
   if (versionVarAssocTable.find(name) != versionVarAssocTable.end()) {
      result = versionVarAssocTable[name];
   } 
   return result;
}

/* APIs for Which field of a variable is referred */
void storeVarFieldReferenceTable(string referred, string referer) {
   size_t pos = 0;
   vector<string> tokens; 
   while ((pos = referred.find(".")) != std::string::npos) {
      tokens.push_back(referred.substr(0, pos));
      referred.erase(0, pos + 1);
   }
   tokens.push_back(referred);

   if (tokens.size() == 2) {
      varFeildReferenceTable[tokens[0]][tokens[1]] = referer;
   } else {
      yyerror("Invalid function call");
   }
}

string findVarFieldReferredName(string name, string field) {
   unordered_map<string, string> referredFieldName = varFeildReferenceTable[name];
   if (referredFieldName.size() == 0) {
      return "";
   }

   if (referredFieldName.find(field) == referredFieldName.end()) {
      return "";
   }

   return referredFieldName[field];
}

/* APIs for handling output related things */
void storeOutputVarField(string output, string field) {
   outputVarFieldTable[output] = field;
}

unordered_map<string, string> getOutputVarFieldTable() {
   return outputVarFieldTable;
}

/* APIs for storing and searching in NOT EXIST reasoning opts in WHERE clause */
void storenotExistSpecifiedVarsSet(string name) {
   notExistSpecifiedVarsSet.insert(name);
}

void clearnotExistSpecifiedVarsSet() {
   notExistSpecifiedVarsSet.clear();
}

bool isVariableSpecifiedInNotExist(string name) {
   return (notExistSpecifiedVarsSet.find(name) != notExistSpecifiedVarsSet.end());
}
