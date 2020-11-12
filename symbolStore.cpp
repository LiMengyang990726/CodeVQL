
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolStore.h"
using namespace std;

unordered_map<string, string> symbolTable;
unordered_map<string, string> varDeclarationTable;
unordered_map<string, char> ruleReferenceTable;

/* APIs for Symbol Table */
void initializeSymbolTable() {
   symbolTable["MethodAccess"] = "(id: string, location: string, getMethod: string)";
   symbolTable["Method"] = "(id: string, hasName: string)";
   return;
}

void findSymbol(string key) {
   if (symbolTable.find(key) != symbolTable.end()) {
      cout << symbolTable[key];
   } else {
      cout << "Error: cannot find the corresponding symbol in symbol table";
   }
}

string findSymbolWithoutPrint(string key) {
   string result = "";
   if (symbolTable.find(key) != symbolTable.end()) {
      result = symbolTable[key];
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