#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
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
unordered_set<string> closureTypes;

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

void writeVersion(const string& name) {
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

    string result;
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

void writeVersionCombInRule() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);

    string result;
    result += "VersionComb(";
    vector<string> versions = getVersions(); 
    for (auto it = versions.rbegin(); it != versions.rend(); it++) {
        result += *it;
        result += ", ";
    }
    result = result.substr(0, result.size()-2);
    result += ")";

    mainDLFile << result << endl;
    mainDLFile.close();
}

void writeOutputDecl() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ".decl " << OUTPUT_NAME << EvoMeObjToSouffleOutput(getOutputVarFieldTable()) << endl;
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
    mainDLFile << OUTPUT_NAME << EvoMeObjToSouffleRuleBegin(getOutputVarFieldTable()) << ":- ";
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

void writeOrRule() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "; ";
    mainDLFile.close();
}

void writeLeftBracket() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << "(";
    mainDLFile.close();
}

void writeRightBracket() {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << ")";
    mainDLFile.close();
}

void writeEquality(const string& a, const string& b) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << a << "=" << b;
    mainDLFile.close();
}

void writeInequality(const string& a, const string& b) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << a << "!=" << b;
    mainDLFile.close();
}

void writeArithmethics(const string& var, const string& comparision, const string& value) {
    ofstream mainDLFile;
    mainDLFile.open(mainDLFileName, ios_base::app);
    mainDLFile << var << comparision << value;
    mainDLFile.close();
}

void writeIncludeEvoMeUniqueMethod() {
    if (!closureTypes.empty()) {
        ofstream mainDLFile;
        mainDLFile.open(mainDLFileName, ios_base::app);
        mainDLFile << "#include \"" << EVOME_UNIQUE_METHOD_REL_DL_FILE_NAME << "\"" << endl 
                   << endl;
        mainDLFile.close();
    }
}

void writeClosure(const string& type) {
    if (closureTypes.find(type) != closureTypes.end()) {
        return;
    }
    if (!isTypeValidClosure(type)) {
        yyerror("Closure on the give type cannot be obtained");
        return;
    }
    closureTypes.insert(type);
    ofstream factTypesDL;
    factTypesDL.open(EVOME_UNIQUE_METHOD_REL_DL_FILE_NAME_ABS, ios_base::app);
    string intermediateStr = repeatString(string(1, IGNORED_SOUFFLE_FIELD), getFieldLen(type)-3);
    if (!intermediateStr.empty()) {
        intermediateStr += ", ";
    }
    factTypesDL << ".decl " << type << "Closure(er: String, ee: String, version: Version)" << endl;
    factTypesDL << type << "Closure(er, ee, version) :- " << type << "(er, ee, " << intermediateStr << "version)." << endl;
    factTypesDL << type << "Closure(er, ee, version) :- " << type << "(er, m, " << intermediateStr << "version), " << type << "(m, " << intermediateStr << "ee, version)." << endl << endl;
    factTypesDL.close();
}

void writeRule(const string& name, string field, string value) {
    ofstream mainDLFile;
    string result;
    mainDLFile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    try {
        mainDLFile.open(mainDLFileName, ios_base::app);
        string type = findVarDeclaration(name);
        vector<pair<string, string> > fieldsVector = destructSouffleDecl(EvoMeObjToSouffleDecl(type));
        string version = findVersionVarAssociation(name);

        // Check 1: If the rule is closure
        if (field.back() == CLOSURE_SUFFIX) {
            type += CLOSURE;
        }

        // Check 2: This part of rule is enclosed in "WHERE NOT EXIST (...)"
        if (isVariableSpecifiedInNotExist(name)) {
            result += "!";
        }

        // Check 3: This part of rule is enclosed in "WHERE ... (... SUCH THAT NOT ...)"
        for (const pair<string, string> &fieldPair : fieldsVector) {
            string currField = fieldPair.first;
            string currFieldClosure = currField + "+";
            string referer;
            if (!findVarFieldReferredName(name, currField).empty()) {
                referer = findVarFieldReferredName(name, currField);
            } else if (!findVarFieldReferredName(name, currFieldClosure).empty()) {
                referer = findVarFieldReferredName(name, currField + "+");
            }
            if (!referer.empty() && referer[0] == '!') {
                result += "!";
            }
        }

        // Step 1: Write the TYPE of this part of rule
        result += type;
        result += "(";

        // Step 2: Write this part of rule with references shown
        for (const pair<string, string> &fieldPair : fieldsVector) {
            string currField = fieldPair.first;
            string currFieldClosure = currField + "+";
            if (currField == field || currFieldClosure == field) {
                if (!value.empty() && value[0] == '!') {
                    result += value.substr(1, value.length() - 1);
                } else {
                    result += value;
                }
            } else if (!findVarFieldReferredName(name, currField).empty()) {
                string referer = findVarFieldReferredName(name, currField);
                if (referer[0] == '!') {
                    result += referer.substr(1, referer.length() - 1);
                } else {
                    result += referer;
                }
            } else if (!findVarFieldReferredName(name, currFieldClosure).empty()) {
                string referer = findVarFieldReferredName(name, currFieldClosure);
                if (referer[0] == '!') {
                    result += referer.substr(1, referer.length() - 1);
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
        result[result.length() - 1] = ')';
        result += ("," + VERSION_DECL_PREFIX + version + "(" + version + ")");
        mainDLFile << result;
        // std::cout << result << std::endl;
        mainDLFile.close();
    } catch (std::ofstream::failure &e) {
        std::cerr << "Exceptions when writing following contents to file " << mainDLFileName  << "\n"
                  <<  result << std::endl;
    }
}