
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
using namespace std;

unordered_map<string, string> symbolTable;
unordered_map<string, string> varSymbolTable;

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
}

void storeVarSymbolTable(char* type, char* name) {
   string typeStr(type);
   string nameStr(name);
   varSymbolTable[typeStr] = nameStr;
}
