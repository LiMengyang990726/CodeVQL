#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include "populateVersionDL.h"
#include "utils.h"

#define VERSION_DL_PREFIX "rules/version"
#define VERSION_DL_POSTFIX ".dl"
#define VERSION_OUTPUT_PREFIX "output/"
#define VERSION_OUTPUT_POSTFIX ".facts"
#define PLAIN_VERSION_REGEX "^\"[a-zA-Z0-9]{40}\"$"
#define NTH_ANCESTOR_REGEX "^\"[a-zA-Z0-9]{40}~[0-9]+\"$"
#define NTH_PARENT_REGEX "^\"[a-zA-Z0-9]{40}\\^[0-9]+\"$"
using namespace std;

int numVersions = 1;
bool isCommitDistDeclared = false;

void writeVersionDL(string name, string range) {
    writeVersionDLTemplate(name);

    if (regex_match(range, regex(PLAIN_VERSION_REGEX))) {
        writeVersionDLInput(name, range);
        writeVersionDLDirect(name);
    } else if (regex_match(range, regex(NTH_ANCESTOR_REGEX))) {
        string start = range.substr(0, range.find("~")) + "\"";
        string nStr = range.substr(range.find("~")+1, range.size());
        stringstream ss(nStr);
        int n = 0;
        ss >> n;
        writeVersionDLNthAncestor(name, start, n);
    } else if (regex_match(range, regex(NTH_PARENT_REGEX))) {
        string start = range.substr(0, range.find("^")) + "\"";
        string nStr = range.substr(range.find("^")+1, range.size());
        stringstream ss(nStr);
        int n = 0;
        ss >> n;
        writeVersionDLInput(name, start);
        writeVersionDLNthParent(name, start, n);
    } else {
        yyerror("invalid version selection");
    }

    writeVersionDLOutput(name);
}

void writeVersionDL(string name, vector<string> ranges) {
    writeVersionDLTemplate(name);

    for (string range : ranges) {
        if (regex_match(range, regex(PLAIN_VERSION_REGEX))) {
            writeVersionDLInput(name, range);
            writeVersionDLDirect(name);
        } else if (regex_match(range, regex(NTH_ANCESTOR_REGEX))) {
            string start = range.substr(0, range.find("~")) + "\"";
            string nStr = range.substr(range.find("~")+1, range.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLNthAncestor(name, start, n);
            isCommitDistDeclared = true;
        } else if (regex_match(range, regex(NTH_PARENT_REGEX))) {
            string start = range.substr(0, range.find("^")) + "\"";
            string nStr = range.substr(range.find("^")+1, range.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLInput(name, start);
            writeVersionDLNthParent(name, start, n);
        } else {
            yyerror("invalid version selection");
        }
        numVersions++;
    }

    writeVersionDLOutput(name);
}

void writeVersionDLBranch(string name, string compBranch, string baseBranch) {
    writeVersionDLTemplate(name);
    writeVersionDLBranchComp(name, compBranch, baseBranch);
    writeVersionDLOutput(name);
}

void writeVersionDLDirect(string name) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- InputVersion" << numVersions << "(version)." << endl << endl;

    versionDL.close();
}

void writeVersionDLNthAncestor(string name, string start, int n) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    if (!isCommitDistDeclared) {
        versionDL << ".decl CommitDist(child: Version, parent: Version, index: Int)" << endl;
        versionDL << "CommitDist(a, x, n+1) :- CommitDist(a, y, n), Parent(y, x, 0)." << endl << endl;
    }
    versionDL << "CommitDist(" << start << ", " << start << ", 0)." << endl;

    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- CommitDist(" << start << ", version, " << n << ")." << endl << endl;

    versionDL.close();
}

void writeVersionDLNthParent(string name, string start, int n) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- Parent(" << start << ", version, "<< (n-1) << "), InputVersion" << numVersions << "(" << start << ")." << endl << endl;

    versionDL.close();
}

void writeVersionDLBranchComp(string name, string compBranch, string baseBranch) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- Branch(version, " << baseBranch << "), !Branch(version, " << compBranch << ")." << endl << endl;

    versionDL.close();
}

void writeVersionDLTemplate(string name) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);
    
    versionDL << "#include \"types.dl\"" << endl;
    versionDL << "#include \"objectMapping.dl\"" << endl;

    versionDL << ".decl Parent(child: Version, parent: Version, index: Int)" << endl;
    versionDL << ".input Parent" << endl << endl;

    versionDL << ".decl Branch(commit: Version, branch: String)" << endl;
    versionDL << ".input Branch" << endl << endl;

    string varName = "SelectedVersion" + name;
    versionDL << ".decl " << varName << "(version: Version)" << endl << endl;

    versionDL.close();
}

void writeVersionDLInput(string name, string version) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    versionDL << ".decl InputVersion" << numVersions << "(version: Version)" << endl;
    versionDL << "InputVersion" << numVersions << "(" << version << ")." << endl << endl;

    versionDL.close();
} 

void writeVersionDLOutput(string name) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    string varName = "SelectedVersion" + name;
    versionDL << ".output " << varName << "(IO=file, filename=\"" << VERSION_OUTPUT_PREFIX << varName << VERSION_OUTPUT_POSTFIX << "\")" << endl;

    versionDL.close();
}