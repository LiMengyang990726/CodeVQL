
#include <stdarg.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <vector>
#include <sstream>
#include "utils.h"

/* Utility for flex */
void yyerror(char *s, ...) {
  extern int yylineno;
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

vector<pair<string, string> > destructSouffleDecl(string fields) {
    fields = fields.substr(1, fields.length()-1);
    vector<pair<string, string> > result;
    stringstream ss(fields);
    pair<string, string> curr;
    for (string i; ss >> i;) {
        if (i[i.length()-1] == ':') {
            curr.first = i.substr(0, i.length()-1);
        } else {
            curr.second = i.substr(0, i.length()-1);
            result.push_back(curr);
        }
        
    }

    return result;
}

string constructSouffleDecl(vector<pair<string, string> > fieldPairs) {
    string result = "(";
    unordered_set<string> seenFields;
    for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
        if (seenFields.find(it->first) != seenFields.end()) {
            continue;
        }
        seenFields.insert(it->first);
        result += it->first + ": " + it->second + ", ";
    }
    result = result.substr(0, result.length()-2);
    result += ")";
    return result;
}

string constructSouffleRuleBegin(vector<string> fields) {
    string result = "(";
    unordered_set<string> seenFields;
    for (string field : fields) {
        if (seenFields.find(field) != seenFields.end()) {
            continue;
        }
        seenFields.insert(field);
        result += field + ", ";
    }
    result = result.substr(0, result.length()-2);
    result += ")";
    return result;
}