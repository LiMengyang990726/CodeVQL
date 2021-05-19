#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>
#include "populateMainSouffle.h"
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
    mainDLFile << "#include \""<< BASE_TYPES_DL_FILE_NAME << "\"" << endl;
    mainDLFile << "#include \"" << FACT_TYPES_DL_FILE_NAME << "\"" << endl
               << endl;
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

void writeOutputDecl() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << OUTPUT_NAME << CodeVQLObjToSouffleOutput(getoutputVarsSet()) << endl;
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
    mainDLFile << OUTPUT_NAME << CodeVQLObjToSouffleRuleBegin(getoutputVarsSet()) << ":- ";
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
    string fields = CodeVQLObjToSouffleDecl(type);
    vector<pair<string, string> > fieldsVector = destructSouffleDecl(fields);
    string version = findVersionVarAssociation(name);

    string result = "";

    // Check 1: The whole rule contains exactly one "VersionComb(v1, ...)"
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

    // Check 2: This part of rule is enclosed in "WHERE NOT EXIST (...)"
    if (isVariableSpecifiedInNotExist(name)) {
        result += "!";
    }

    // Check 3: This part of rule is enclosed in "WHERE ... (... SUCH THAT NOT ...)"
    for (pair<string, string> fieldPair : fieldsVector) {
        string currField = fieldPair.first.c_str();
        string referer = findVarFieldReferredName(name, currField);
        if (referer != "" && referer[0] == '!') {
            result += "!";
        }
    }
 
    // Step 1: Write the TYPE of this part of rule
    result += type;
    result += "(";

    // Step 2: Write this part of rule with references shown
    for (pair<string, string> fieldPair : fieldsVector) {
        string currField = fieldPair.first.c_str();
        if (currField == field) {
            if (value != "" && value[0] == '!') {
                result += value.substr(1,value.length()-1);
            } else {
                result += value;
            }
        } else if (findVarFieldReferredName(name, currField) != "") {
            string referer = findVarFieldReferredName(name, currField);
            if (referer[0] == '!') {
                result += referer.substr(1,referer.length()-1);
            } else {
                result += referer;
            }
        } else if (currField == "version") {
            result += version; 
        } else {
            result += "_";
        }
        result += ",";
    }

    // Step 3: Enclose this part of rule
    result[result.length()-1] = ')';
    result += ("," + VERSION_DECL_PREFIX + version + "(" + version + ")");
    mainDLFile << result;

    mainDLFile.close();
}