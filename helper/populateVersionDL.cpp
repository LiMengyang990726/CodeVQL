#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>
#include "populateVersionDL.h"
#include "constants.h"
#include "utils.h"

#define VERSION_DL_PREFIX "rules/SelectedVersion"
#define VERSION_DL_POSTFIX ".dl"
#define VERSION_DL VERSION_DL_PREFIX VERSION_DL_POSTFIX
#define VERSION_OUTPUT_PREFIX "../.facts/20-deps/"
#define VERSION_OUTPUT_POSTFIX ".facts"
#define PLAIN_VERSION_REGEX "^\"[a-zA-Z0-9]{40}\"$"
#define NTH_ANCESTOR_REGEX "^\"[a-zA-Z0-9]{40}~[0-9]+\"$"
#define NTH_PARENT_REGEX "^\"[a-zA-Z0-9]{40}\\^[0-9]+\"$"
#define VARNAME_PREFIX "SelectedVersion"
using namespace std;

int numVersions = 1;
bool isCommitDistDeclared = false;

void writeVersionsCombination(vector<string> inputs)
{
    string filename = VERSION_DL;
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);
    versionDL << "#include \"types.dl\"" << endl << endl;

    for (string input : inputs) {
        string varName = VARNAME_PREFIX + input;
        versionDL << ".decl " << varName << "(version: Version)" << endl;
        versionDL << ".input " << varName << endl << endl;
    }

    versionDL << ".decl " << VARNAME_PREFIX << "(";
    for (int i = 0; i < inputs.size(); i++) {
        char c = i + 'a';
        if (i == inputs.size() - 1) {
            versionDL << c << ": Version)" << endl;
        } else {
            versionDL << c << ": Version, ";
        }
    }

    versionDL << VARNAME_PREFIX << "(";
    for (int i = 0; i < inputs.size(); i++) {
        char c = i + 'a';
        if (i == inputs.size() - 1) {
            versionDL << c << ") :- ";
        } else {
            versionDL << c << ", ";
        }
    }

    for (int i = 0; i < inputs.size(); i++) {
        char c = i + 'a';
        string varName = VARNAME_PREFIX + inputs[i];
        if (i == inputs.size() - 1) {
            versionDL << varName << "(" << c << "). " << endl;
        } else {
            versionDL << varName << "(" << c << "), ";
        }
    }
    versionDL << ".output " << VARNAME_PREFIX << "(IO=file, filename=\"" << VERSION_OUTPUT_PREFIX << VARNAME_PREFIX << VERSION_OUTPUT_POSTFIX << "\")" << endl;
}

void writeVersionDL(string varName, string version)
{
    string filename = VERSION_DL_PREFIX + varName + VERSION_DL_POSTFIX;
    writeVersionDLTemplate(filename, varName);

    if (regex_match(version, regex(PLAIN_VERSION_REGEX)))
    {
        writeVersionDLInput(filename, varName, version);
        writeVersionDLDirect(filename, varName);
    }
    else if (regex_match(version, regex(NTH_ANCESTOR_REGEX)))
    {
        string commitId = version.substr(0, version.find("~")) + "\"";
        string nStr = version.substr(version.find("~") + 1, version.size());
        stringstream ss(nStr);
        int n = 0;
        ss >> n;
        writeVersionDLNthAncestor(filename, varName, commitId, n);
    }
    else if (regex_match(version, regex(NTH_PARENT_REGEX)))
    {
        string commitId = version.substr(0, version.find("^")) + "\"";
        string nStr = version.substr(version.find("^") + 1, version.size());
        stringstream ss(nStr);
        int n = 0;
        ss >> n;
        writeVersionDLInput(filename, varName, commitId);
        writeVersionDLNthParent(filename, varName, commitId, n);
    }
    else
    {
        yyerror("invalid version selection");
        return;
    }

    writeVersionDLOutput(filename, varName);
}

void writeVersionDL(string varName, int type, vector<string> versions)
{
    string filename = VERSION_DL_PREFIX + varName + VERSION_DL_POSTFIX;
    writeVersionDLTemplate(filename, varName);

    if (type == MULTIPLE_VERSIONS_TYPE_1)
    {
        for (string version : versions)
        {
            if (regex_match(version, regex(PLAIN_VERSION_REGEX)))
            {
                writeVersionDLInput(filename, varName, version);
                writeVersionDLDirect(filename, varName);
            }
            else if (regex_match(version, regex(NTH_ANCESTOR_REGEX)))
            {
                string commitId = version.substr(0, version.find("~")) + "\"";
                string nStr = version.substr(version.find("~") + 1, version.size());
                stringstream ss(nStr);
                int n = 0;
                ss >> n;
                writeVersionDLNthAncestor(filename, varName, commitId, n);
                isCommitDistDeclared = true;
            }
            else if (regex_match(version, regex(NTH_PARENT_REGEX)))
            {
                string commitId = version.substr(0, version.find("^")) + "\"";
                string nStr = version.substr(version.find("^") + 1, version.size());
                stringstream ss(nStr);
                int n = 0;
                ss >> n;
                writeVersionDLInput(filename, varName, commitId);
                writeVersionDLNthParent(filename, varName, commitId, n);
            }
            else
            {
                yyerror("invalid version selection");
                return;
            }
            numVersions++;
        }
    }
    else if (type == MULTIPLE_VERSIONS_TYPE_2)
    {
        if (versions.size() != 2)
        {
            yyerror("invalid branch comparison in multiple versions selection");
            return;
        }

        // Check the fromCommitId
        string fromCommitId = versions[0];
        bool isFromCommitIdRegex = false;
        if (regex_match(fromCommitId, regex(NTH_ANCESTOR_REGEX)))
        {
            string commitId = fromCommitId.substr(0, fromCommitId.find("~")) + "\"";
            string nStr = fromCommitId.substr(fromCommitId.find("~") + 1, fromCommitId.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLNthAncestor(filename, varName + "from", commitId, n);
            isCommitDistDeclared = true;
            isFromCommitIdRegex = true;
        }
        else if (regex_match(fromCommitId, regex(NTH_PARENT_REGEX)))
        {
            string commitId = fromCommitId.substr(0, fromCommitId.find("^")) + "\"";
            string nStr = fromCommitId.substr(fromCommitId.find("^") + 1, fromCommitId.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLInput(filename, varName + "from", commitId);
            writeVersionDLNthParent(filename, varName + "from", commitId, n);
            isFromCommitIdRegex = true;
        }
        else if (!regex_match(fromCommitId, regex(PLAIN_VERSION_REGEX)))
        {
            yyerror("invalid version selection");
            return;
        }

        // Check the baseCommitId
        string baseCommitId = versions[1];
        bool isBaseCommitIdRegex = false;
        if (regex_match(baseCommitId, regex(NTH_ANCESTOR_REGEX)))
        {
            string commitId = baseCommitId.substr(0, baseCommitId.find("~")) + "\"";
            string nStr = baseCommitId.substr(baseCommitId.find("~") + 1, baseCommitId.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLNthAncestor(filename, varName + "base", commitId, n);
            isCommitDistDeclared = true;
            isBaseCommitIdRegex = true;
        }
        else if (regex_match(baseCommitId, regex(NTH_PARENT_REGEX)))
        {
            string commitId = baseCommitId.substr(0, baseCommitId.find("^")) + "\"";
            string nStr = baseCommitId.substr(baseCommitId.find("^") + 1, baseCommitId.size());
            stringstream ss(nStr);
            int n = 0;
            ss >> n;
            writeVersionDLInput(filename, varName + "base", commitId);
            writeVersionDLNthParent(filename, varName + "base", commitId, n);
            isBaseCommitIdRegex = true;
        }
        else if (!regex_match(baseCommitId, regex(PLAIN_VERSION_REGEX)))
        {
            yyerror("invalid version selection");
            return;
        }

        writeVersionDLComp(filename, varName,
                           isFromCommitIdRegex ? VARNAME_PREFIX + varName + "from" : fromCommitId,
                           isBaseCommitIdRegex ? baseCommitId + varName + "base" : baseCommitId);
    }
    else
    {
        yyerror("invalid multiple versions selection type");
        return;
    }

    writeVersionDLOutput(filename, varName);
}

void writeVersionDLDirect(string filename, string varName)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << VARNAME_PREFIX << varName << "(version) :- InputVersion" << numVersions << "(version)." << endl
              << endl;

    versionDL.close();
}

void writeVersionDLNthAncestor(string filename, string varName, string commitId, int n)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    if (!isCommitDistDeclared)
    {
        versionDL << ".decl CommitDist(child: Version, parent: Version, index: Int)" << endl;
        versionDL << "CommitDist(a, x, n+1) :- CommitDist(a, y, n), Parent(y, x, 0)." << endl
                  << endl;
    }
    versionDL << "CommitDist(" << commitId << ", " << commitId << ", 0)." << endl;

    versionDL << VARNAME_PREFIX << varName << "(version) :- CommitDist(" << commitId << ", version, " << n << ")." << endl
              << endl;

    versionDL.close();
}

void writeVersionDLNthParent(string filename, string varName, string commitId, int n)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << VARNAME_PREFIX << varName << "(version) :- Parent(" << commitId << ", version, " << (n - 1) << "), InputVersion" << numVersions << "(" << commitId << ")." << endl
              << endl;

    versionDL.close();
}

void writeVersionDLComp(string filename, string varName, string from, string base)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << VARNAME_PREFIX << varName << "(version) :- VersionInRange(version, " << from << ", " << base << ")." << endl
              << endl;

    versionDL.close();
}

void writeVersionDLTemplate(string filename, string varName)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << "#include \"types.dl\"" << endl;
    versionDL << "#include \"objectMapping.dl\"" << endl;

    versionDL << ".decl Parent(child: Version, parent: Version, index: Int)" << endl;
    versionDL << ".input Parent" << endl
              << endl;

    versionDL << ".decl Reachable(a:Version, b:Version)" << endl;
    versionDL << "Reachable(a, b) :- Parent(a, b, _)." << endl;
    versionDL << "Reachable(a, x) :- Parent(a, b, _), Reachable(b, x)." << endl
              << endl;

    versionDL << ".decl VersionInRange(x: Version, a:Version, b:Version)" << endl;
    versionDL << "VersionInRange(x, a, b) :- Reachable(b, x), Parent(_,a,_), !Reachable(a, x)." << endl;
    versionDL << "VersionInRange(x, a, b) :- Reachable(b, x), Parent(a,_,_), !Reachable(a, x)." << endl
              << endl;

    versionDL << ".decl " << VARNAME_PREFIX << varName << "(version: Version)" << endl;
    versionDL << ".decl " << VARNAME_PREFIX << varName << "from" << "(version: Version)" << endl;
    versionDL << ".decl " << VARNAME_PREFIX << varName << "base" << "(version: Version)" << endl
              << endl;

    versionDL.close();
}

void writeVersionDLInput(string filename, string varName, string commitId)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << ".decl InputVersion" << numVersions << "(version: Version)" << endl;
    versionDL << "InputVersion" << numVersions << "(" << commitId << ")." << endl
              << endl;

    versionDL.close();
}

void writeVersionDLOutput(string filename, string varName)
{
    ofstream versionDL;
    versionDL.open(filename, ios_base::app);

    versionDL << ".output " << VARNAME_PREFIX << varName << "(IO=file, filename=\"" << VERSION_OUTPUT_PREFIX << VARNAME_PREFIX << varName << VERSION_OUTPUT_POSTFIX << "\")" << endl;

    versionDL.close();
}