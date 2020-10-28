#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "symbolTable.h"
using namespace std;

string outputVar;
static int symbolIdx = 0;
int symbolOffset = 65;

char getNextSymbol() {
    char result = symbolOffset + symbolIdx;
    symbolIdx++;
    return result;
}

void storeOutputVar(string output) {
    outputVar = output;
}

void printFormulaLHS() {
    printf("%s(%c) :- ", outputVar.c_str(), getNextSymbol());
}

vector<pair<string, string> > split(string fields) {
    fields = fields.substr(1, fields.length()-2);
    vector<pair<string, string> > result;
    stringstream ss(fields);
    pair<string, string> curr;
    for (string i; ss >> i;) {
        if (i[i.length()-1] == ':') {
            curr.first = i.substr(0, i.length()-1);
        } else {
            curr.second = i;
            result.push_back(curr);
        }
        if (ss.peek() == ', ')
            ss.ignore();
    }

    return result;
}

void replace(char* name, char* field, char* value) {
    string type = findVarDeclSymbolWithoutPrint(name);
    const char* typeCharP = type.c_str();

    string referenceSymbol = findReferenceSymbol(name);
    bool isReferenced = !referenceSymbol.empty();

    string fields = findSymbolWithoutPrint(type);
    const char* fieldsCharP = fields.c_str();

    vector<pair<string, string> > fieldsVector = split(fields);

    string result = type;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        const char* currFieldCharP = fieldPair.first.c_str();
        if (strcmp(field, currFieldCharP) == 0) {
            result += value;
        } else if (strcmp("location", currFieldCharP) == 0) {
            result += symbolOffset; // as the initial symbol will be a
        } else if (isReferenced && strcmp("id", currFieldCharP) == 0) {
            result += referenceSymbol;
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    const char* resultCharP = result.c_str();
    printf("%s\n", resultCharP);
}

char replaceCreateReference(char* name, char* field) {
    string type = findVarDeclSymbolWithoutPrint(name);
    const char* typeCharP = type.c_str();

    string referenceSymbol = findReferenceSymbol(name);
    bool isReferenced = !referenceSymbol.empty();

    string fields = findSymbolWithoutPrint(type);
    const char* fieldsCharP = fields.c_str();

    vector<pair<string, string> > fieldsVector = split(fields);

    char newReferenceSymbol;
    string result = type;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        const char* currFieldCharP = fieldPair.first.c_str();
        if (strcmp(field, currFieldCharP) == 0) {
            newReferenceSymbol = getNextSymbol();
            result += newReferenceSymbol;
        } else if (strcmp("location", currFieldCharP) == 0) {
            result += symbolOffset; // as the initial symbol will be a
        } else if (isReferenced && strcmp("id", currFieldCharP) == 0) {
            result += referenceSymbol;
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    const char* resultCharP = result.c_str();
    printf("%s\n", resultCharP);
    return newReferenceSymbol;
}
