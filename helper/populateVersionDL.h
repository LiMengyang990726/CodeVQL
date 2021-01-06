#include <string>

using namespace std;

void writeVersionsCombination(vector<string> inputs);

void writeVersionDL(string varName, string version);
void writeVersionDL(string varName, int type, vector<string> versions);

void writeVersionDLDirect(string filename, string varName);
void writeVersionDLNthAncestor(string filename, string varName, string commitId, int n);
void writeVersionDLNthParent(string filename, string varName, string commitId, int n);
void writeVersionDLComp(string filename, string varName, string from, string base);

void writeVersionDLTemplate(string filename, string varName);
void writeVersionDLInput(string filename, string varName, string commitId);
void writeVersionDLOutput(string filename, string varName);
