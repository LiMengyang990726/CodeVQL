#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "populateMainDL.h"
#include "constants.h"
#include "symbolStore.h"
#include "utils.h"

#define mainDLFileName "rules/main.dl"
using namespace std;

const string VERSION_DECL_PREFIX = "SelectedVersion";
static bool isTemplateWritten = false;
static bool isVersionCombWritten = false;

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

void writeVersionComb() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl VersionComb(";  

    string result = "";
    vector<string> versions = getVersions(); 
    for (auto it = versions.rbegin(); it != versions.rend(); it++) {
        result += *it;
        result += ": Version, ";
    }
    result = result.substr(0, result.size()-2);
    result += ") ";

    mainDLFile << result << endl;
    mainDLFile << ".input VersionComb" << endl << endl;
    mainDLFile.close();
}

void writeDecl(string type) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << type << QLObjToDLDecl(type) << endl;
    mainDLFile.close();
}

void writeOutputDecl() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << OUTPUT_NAME << QLObjToDLOutput(getOutputVars()) << endl;
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
    mainDLFile << ".output " << OUTPUT_NAME << endl;
    mainDLFile.close();
}

void writeRuleBegin() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << OUTPUT_NAME << QLObjToDLRuleBegin(getOutputVars()) << ":- ";
    mainDLFile.close();
}

void writeRuleTermination() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "." << endl << endl;
    mainDLFile.close();
}

void writeNegationRule() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "!";
    mainDLFile.close(); 
}

void writeParallelRule() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ", ";
    mainDLFile.close();
}

void writeEquality(string a, string b) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << a << "=" << b;
    mainDLFile.close();
}

void writeInequality(string a, string b) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << a << "!=" << b;
    mainDLFile.close();
}

void writeArithmethics(string var, string comparision, string value) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << var << comparision << value;
    mainDLFile.close();
}

void writeRule(string name, string field, string value) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);

    string type = findVarDeclaration(name);
    string version = findVersionVarAssociation(name);

    string fields = QLObjToDLDecl(type);
    vector<pair<string, string> > fieldsVector = destructDLDecl(fields);

    string result = "";

    if (!isVersionCombWritten) {
        result += "VersionComb(";
        vector<string> versions = getVersions(); 
        for (auto it = versions.rbegin(); it != versions.rend(); it++) {
            result += *it;
            result += ", ";
        }
        result = result.substr(0, result.size()-2);
        result += "), ";
        isVersionCombWritten = true;
    } 

    if (isVariableSpecifiedInNotExist(name)) {
        result += "!";
    }

    for (pair<string, string> fieldPair : fieldsVector) {
        string currField = fieldPair.first.c_str();
        string referedName = findVarFieldReferredName(name, currField);
        if (referedName != "" && referedName[0] == '!') {
            result += "!";
        }
    }
    
    result += type;

    result += "(";
    for (pair<string, string> fieldPair : fieldsVector) {
        string currField = fieldPair.first.c_str();
        if (currField == field) {
            if (value.empty()) {
                string referedName = findVarFieldReferredName(name, currField);
                if (referedName[0] == '!') {
                    result += referedName.substr(1,referedName.length()-1);
                } else {
                    result += referedName;
                }
            } else if (value != "any") {
                result += value;
            } else if (value == "any") {
                result += "_";
            }
        } else if (findVarFieldReferredName(name, currField) != "") {
            string referedName = findVarFieldReferredName(name, currField);
            if (referedName[0] == '!') {
                result += referedName.substr(1,referedName.length()-1);
            } else {
                result += referedName;
            }
        } else if (currField == "version") {
            result += version; 
        } else {
            result += "_";
        }
        result += ",";
    }
    result[result.length()-1] = ')';
    result += ("," + VERSION_DECL_PREFIX + version + "(" + version + ")");
    mainDLFile << result;

    mainDLFile.close();
}