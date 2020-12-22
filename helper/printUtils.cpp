#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "printUtils.h"
#include "symbolStore.h"
#include "utils.h"
using namespace std;

static int symbolIdx = 0;
int symbolOffset = 65;

/* Common APIs */
char getNextSymbol() {
    char result = symbolOffset + symbolIdx;
    symbolIdx++;
    return result;
}

/* Output Print APIs */
void printFormulaLHS() {
    printf("%s(%c) :- ", getOutputVar().c_str(), getNextSymbol());
}

void printRule(char* name, char* field, char* value) {
    string type = findVarDeclaration(name);
    const char* typeCharP = type.c_str();

    string ruleReference = findRuleReference(name);
    bool isReferenced = !ruleReference.empty();

    string fields = QLObjToDLDecl(type);
    const char* fieldsCharP = fields.c_str();

    vector<pair<string, string> > fieldsVector = destructDLDecl(fields);

    string result = type;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        const char* currFieldCharP = fieldPair.first.c_str();
        if (strcmp(field, currFieldCharP) == 0) {
            result += value;
        } else if (strcmp("location", currFieldCharP) == 0) {
            result += symbolOffset; // as the initial symbol will be a
        } else if (isReferenced && strcmp("id", currFieldCharP) == 0) {
            result += ruleReference;
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    const char* resultCharP = result.c_str();
    printf("%s\n", resultCharP);
}

char printRuleReturnReference(char* name, char* field) {
    string type = findVarDeclaration(name);
    const char* typeCharP = type.c_str();

    string ruleReference = findRuleReference(name);
    bool isReferenced = !ruleReference.empty();

    string fields = QLObjToDLDecl(type);
    const char* fieldsCharP = fields.c_str();

    vector<pair<string, string> > fieldsVector = destructDLDecl(fields);

    char newRuleReference;
    string result = type;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        const char* currFieldCharP = fieldPair.first.c_str();
        if (strcmp(field, currFieldCharP) == 0) {
            newRuleReference = getNextSymbol();
            result += newRuleReference;
        } else if (strcmp("location", currFieldCharP) == 0) {
            result += symbolOffset; // as the initial symbol will be a
        } else if (isReferenced && strcmp("id", currFieldCharP) == 0) {
            result += ruleReference;
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    const char* resultCharP = result.c_str();
    printf("%s\n", resultCharP);
    return newRuleReference;
}
