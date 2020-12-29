#include <string>

using namespace std;

void writeVersionDL(string name, string range);
void writeVersionDL(string name, vector<string> ranges);
void writeVersionDLDirect(string name);
void writeVersionDLNthAncestor(string name, string start, int n);
void writeVersionDLNthParent(string name, string start, int n);
void writeVersionDLTemplate(string name);
void writeVersionDLInput(string name, string version);
void writeVersionDLOutput(string name);
