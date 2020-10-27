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
    vector<pair<string, string> > result;
    stringstream ss(fields);
    for (string i; ss >> i;) {
        vector<string> curr;
        stringstream ss2(i);
        for (string j; ss2 >> j;) {
            curr.push_back(j);
            if (ss2.peek() == ': ') 
                ss2.ignore();
        } 
        result.push_back(make_pair(curr[0], curr[1]));    
        if (ss.peek() == ', ')
            ss.ignore();
    }

    return result;
}

void replace(char* name, char* field, char* value) {
    string fields = findSymbolWithoutPrint(name);
    vector<pair<string, string> > fieldsVector = split(fields);
    string result = name;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        if (fieldPair.second == field) {
            result += value;
            result += ",";
        } else {
            result += "_,";
        }
    }
    result[result.length()-1] = ')';
    cout << result;
}