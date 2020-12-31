#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>
#include "populateVersionDL.h"
#include "constants.h"
#include "utils.h"

#define VERSION_DL_PREFIX "rules/version"
#define VERSION_DL_POSTFIX ".dl"
#define VERSION_OUTPUT_PREFIX "output/"
#define VERSION_OUTPUT_POSTFIX ".facts"
#define PLAIN_VERSION_REGEX "^\"[a-zA-Z0-9]{40}\"$"
#define NTH_ANCESTOR_REGEX "^\"[a-zA-Z0-9]{40}~[0-9]+\"$"
#define NTH_PARENT_REGEX "^\"[a-zA-Z0-9]{40}\\^[0-9]+\"$"
#define EXPERIMENT_BRANCH "experiment"
#define EXPERIMENT_BRANCH_MOST_RECENT_COMMIT "\"f3db11791569ea3e4fd741a2ee243e02561d1008\"" /* todo: obtain it dynamically */
#define MASTER_BRANCH "master"
#define MASTER_BRANCH_MOST_RECENT_COMMIT "\"c0a13d8cc528a449967e83b7d0f4043787597e81\"" /* todo: obtain it dynamically */
using namespace std;

int numVersions = 1;
bool isCommitDistDeclared = false;
unordered_map<string, string> branchMostRecentCommit;

void init() {
    branchMostRecentCommit.insert(make_pair(EXPERIMENT_BRANCH, EXPERIMENT_BRANCH_MOST_RECENT_COMMIT));
    branchMostRecentCommit.insert(make_pair(MASTER_BRANCH, MASTER_BRANCH_MOST_RECENT_COMMIT));
}

void writeVersionDL(string name, string range) {
    init();
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

void writeVersionDL(string name, int type, vector<string> ranges) {
    init();
    writeVersionDLTemplate(name);

    if (type == MULTIPLE_VERSIONS_TYPE_1) {
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
    } else if (type == MULTIPLE_VERSIONS_TYPE_2) {
        if (ranges.size() != 2) {
            yyerror("invalid branch comparison in multiple versions selection");
            return;
        }
        writeVersionDLBranchComp(name, ranges[0], ranges[1]);
    } else if (type == MULTIPLE_VERSIONS_TYPE_3) {
        if (ranges.size() != 2) {
            yyerror("invalid versionInRange parameter in multiple versions selection");
            return;
        }
        writeVersionDLVersionInRange(name, ranges[0], ranges[1]);
    } else {
        yyerror("invalid multiple versions selection type");
        return;
    }
    
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

    if (branchMostRecentCommit.find(compBranch) == branchMostRecentCommit.end() || branchMostRecentCommit.find(baseBranch) == branchMostRecentCommit.end()) {
        yyerror("Cannot get the most recent commit id of the branch comparison in the RANGE clause");
        return;
    }
    string compBranchMostRecentCommit = branchMostRecentCommit.at(compBranch);
    string baseBranchMostRecentCommit = branchMostRecentCommit.at(baseBranch);
    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- Branch(version, " << compBranchMostRecentCommit << "), !Branch(version, " << baseBranchMostRecentCommit << ")." << endl << endl;

    versionDL.close();
}

void writeVersionDLVersionInRange(string name, string endCommit, string startCommit) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);

    string varName = "SelectedVersion" + name;
    versionDL << varName << "(version) :- VersionInRange(version,  " << endCommit << ", " << startCommit << ")." << endl << endl; 

    versionDL.close();
}

void writeVersionDLTemplate(string name) {
    ofstream versionDL;
    versionDL.open(VERSION_DL_PREFIX + name + VERSION_DL_POSTFIX, ios_base::app);
    
    versionDL << "#include \"types.dl\"" << endl;
    versionDL << "#include \"objectMapping.dl\"" << endl;

    versionDL << ".decl Parent(child: Version, parent: Version, index: Int)" << endl;
    versionDL << ".input Parent" << endl << endl;

    versionDL << ".decl Reachable(a:Version, b:Version)" << endl;
    versionDL << "Reachable(a, b) :- Parent(a, b, _)." << endl;
    versionDL << "Reachable(a, x) :- Parent(a, b, _), Reachable(b, x)." << endl << endl;

    versionDL << ".decl VersionInRange(x: Version, a:Version, b:Version)" << endl;
    versionDL << "VersionInRange(x, a, b) :- Reachable(b, x), Parent(_,a,_), !Reachable(a, x)." << endl;
    versionDL << "VersionInRange(x, a, b) :- Reachable(b, x), Parent(a,_,_), !Reachable(a, x)." << endl << endl;

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