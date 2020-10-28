
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolTable.h"
using namespace std;

unordered_map<string, string> symbolTable;
unordered_map<string, string> varSymbolTable;
unordered_map<string, char> ruleReferenceTable;

void initialize() {
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

string findVarDeclSymbolWithoutPrint(string name) {
   string result = "";
   if (varSymbolTable.find(name) != varSymbolTable.end()) {
      result = varSymbolTable[name];
   } 
   return result;
}

string findReferenceSymbol(string name) {
   string result = "";
   if (ruleReferenceTable.find(name) != ruleReferenceTable.end()) {
      result = ruleReferenceTable[name];
   } 
   return result;
}

void storeVarSymbolTable(char* type, char* name) {
   string typeStr(type);
   string nameStr(name);
   varSymbolTable[nameStr] = typeStr;
}

void storeRuleReferenceTable(char referenceSymbol, char* name) {
   string nameStr(name);
   ruleReferenceTable[nameStr] = referenceSymbol;
}