#include <string>

using namespace std;

void init();
void writeVersionDL(string name, string range);
void writeVersionDL(string name, int type, vector<string> ranges);
void writeVersionDLDirect(string name);
void writeVersionDLNthAncestor(string name, string start, int n);
void writeVersionDLNthParent(string name, string start, int n);
void writeVersionDLBranchComp(string name, string compBranch, string baseBranch);
void writeVersionDLVersionInRange(string name, string endCommit, string startCommit);
void writeVersionDLTemplate(string name);
void writeVersionDLInput(string name, string version);
void writeVersionDLOutput(string name);
