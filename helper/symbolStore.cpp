
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolStore.h"
#include "populateMainDL.h"
#include "utils.h"
using namespace std;

unordered_map<string, string> symbolTable;
unordered_map<string, string> varDeclarationTable;
unordered_set<string> versionDeclarationSet;
unordered_map<string, string> versionVarAssocTable;
unordered_map<string, string> ruleReferenceTable;

/* APIs for Symbol Table */
void initializeSymbolTable() {
   symbolTable["MethodAccess"] = "(fqn: String, getMethod: String, version: Version)";
   symbolTable["Method"] = "(fqn: String, hasName: String, version: Version)";
   return;
}

string QLObjToDLDecl(string key) {
   string result = "";
   if (symbolTable.find(key) != symbolTable.end()) {
      result = symbolTable[key];
   } 
   return result;
}

string QLObjToDLOutput(string key) {
   string result = "";
   if (symbolTable.find(key) != symbolTable.end()) {
      string intermediateResult = symbolTable[key];
      vector<pair<string, string> > fieldPairs = destructDLDecl(intermediateResult);
      vector<pair<string, string> > resultPairs;
      for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
         if (it->first != "version") {
            resultPairs.push_back(make_pair(it->first, it->second));
         }
      }
      result = constructDLDecl(resultPairs);
   }
   return result;
}

string QLObjToDLRuleBegin(string key) {
   string result = "";
   if (symbolTable.find(key) != symbolTable.end()) {
      string intermediateResult = symbolTable[key];
      vector<pair<string, string> > fieldPairs = destructDLDecl(intermediateResult);
      vector<string> resultPairs;
      for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
         if (it->first != "version") {
            resultPairs.push_back(it->first);
         }
      }
      result = constructDLRuleBegin(resultPairs);
   }
   return result;
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
   versionDeclarationSet.insert(name);
}

bool findVersionDeclaration(string name) {
   return versionDeclarationSet.find(name) != versionDeclarationSet.end();
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

/* APIs for Rule Reference Table */
void storeRuleReferenceTable(string reference, string name) {
   ruleReferenceTable[name] = reference;
}

string findRuleReference(string name) {
   string result = "";
   if (ruleReferenceTable.find(name) != ruleReferenceTable.end()) {
      result = ruleReferenceTable[name];
   } 
   return result;
}

// TODO: Currently only handle one output case
string outputVar;
void storeOutputVar(string output) {
    outputVar = output;
}

string getOutputVar() {
   return outputVar;
}