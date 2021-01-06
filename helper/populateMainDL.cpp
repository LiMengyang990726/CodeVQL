#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "populateMainDL.h"
#include "symbolStore.h"
#include "utils.h"

#define mainDLFileName "rules/main.dl"
using namespace std;

const string VERSION_DECL_PREFIX = "SelectedVersion";
static bool isTemplateWritten = false;

void writeTemplate() {
    if (isTemplateWritten) {
        return;
    }
    isTemplateWritten = true;
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "#include \"types.dl\"" << endl;
    mainDLFile << "#include \"objectMapping.dl\"" << endl << endl;
    mainDLFile.close();
}

void writeVersion(string name) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl SelectedVersion" << name << "(version: Version)" << endl;  
    mainDLFile << ".input SelectedVersion" << name << endl << endl;
    mainDLFile.close();
}

void writeDecl(string type) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << type << QLObjToDLDecl(type) << endl;
    mainDLFile.close();
}

void writeOutputDecl(string type) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << getOutputVar() << QLObjToDLOutput(type) << endl;
    mainDLFile.close();
}

void writeInput(string type) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".input " << type << endl << endl;
    mainDLFile.close();
}

void writeOutput() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".output " << getOutputVar() << endl;
    mainDLFile.close();
}

void writeRuleBegin() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << getOutputVar() << QLObjToDLRuleBegin(findVarDeclaration(getOutputVar())) << ":- ";
    mainDLFile.close();
}

void writeRuleTermination() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "." << endl << endl;
    mainDLFile.close();
}

void writeParallelRule() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ", " << endl << endl;
    mainDLFile.close();
}

void writeRule(string name, string field, string value) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);

    string type = findVarDeclaration(name);

    string ruleReference = findRuleReference(name);
    bool isReferenced = !ruleReference.empty();

    string fields = QLObjToDLDecl(type);
    vector<pair<string, string> > fieldsVector = destructDLDecl(fields);

    string result = type;
    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        string currFieldCharP = fieldPair.first.c_str();
        if (currFieldCharP == field) {
            if (value.empty()) {
                result += field;
            } else {
                result += value;
            }
        } else if (currFieldCharP == "version") {
            result += ("version" + name);
        } else if (isReferenced) {
            result += ruleReference;
        } else if (currFieldCharP == "fqn") {
            result += "fqn";
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    result += ("," + VERSION_DECL_PREFIX + name + "(version" + name + ")");
    mainDLFile << result;

    mainDLFile.close();
}