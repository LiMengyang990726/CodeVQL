
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
unordered_map<string, char> ruleReferenceTable;

/* APIs for Symbol Table */
void initializeSymbolTable() {
   symbolTable["MethodAccess"] = "(fqn: String, getMethod: String, version: Version)";
   symbolTable["Method"] = "(fqn: String, hasName: String, version: Version)";
   return;
}

void QLObjToDLDecl(string key) {
   if (symbolTable.find(key) != symbolTable.end()) {
      cout << symbolTable[key];
   } else {
      cout << "Error: cannot find the corresponding symbol in symbol table";
   }
}

string QLObjToDLDeclWP(string key) {
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

/* APIs for Var Declaration Table */
void storeVarDeclarationTable(char* type, char* name) {
   string typeStr(type);
   string nameStr(name);
   varDeclarationTable[nameStr] = typeStr;
}

string findVarDeclarationWithoutPrint(string name) {
   string result = "";
   if (varDeclarationTable.find(name) != varDeclarationTable.end()) {
      result = varDeclarationTable[name];
   } 
   return result;
}

/* APIs for Rule Reference Table */
void storeRuleReferenceTable(char referenceSymbol, char* name) {
   string nameStr(name);
   ruleReferenceTable[nameStr] = referenceSymbol;
}

string findRuleReferenceWithoutPrint(string name) {
   string result = "";
   if (ruleReferenceTable.find(name) != ruleReferenceTable.end()) {
      result = ruleReferenceTable[name];
   } 
   return result;
}

string outputVar;
void storeOutputVar(string output) {
    outputVar = output;
}

string getOutputVar() {
   return outputVar;
}