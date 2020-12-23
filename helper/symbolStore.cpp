
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolStore.h"
#include "printUtils.h"
#include "utils.h"
using namespace std;

unordered_map<string, string> symbolTable;
unordered_map<string, string> varDeclarationTable;
unordered_map<string, string> varRangeTable;
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

/* APIs for Variable Range(Version) Table */
void storeVarRangeTable(string name, string range) {
   varRangeTable[name] = range;
}

string findVarRange(string name) {
   string result = "";
   if (varRangeTable.find(name) != varRangeTable.end()) {
      result = varRangeTable[name];
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