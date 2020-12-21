
#include <iostream>
#include <string>
#include <unordered_map>
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

vector<pair<string, string> > destructDLDecl(string fields) {
    fields = fields.substr(1, fields.length()-2);
    vector<pair<string, string> > result;
    stringstream ss(fields);
    pair<string, string> curr;
    for (string i; ss >> i;) {
        if (i[i.length()-1] == ':') {
            curr.first = i.substr(0, i.length()-1);
        } else {
            curr.second = i;
            result.push_back(curr);
        }
        if (ss.peek() == ', ')
            ss.ignore();
    }

    return result;
}

string constructDLDecl(vector<pair<string, string> > fieldPairs) {
    string result = "(";
    for (auto it = fieldPairs.begin(); it != fieldPairs.end(); it++) {
        result += it->first;
        result += ": ";
        result += it->second;
        result += ", ";
    }
    result = result.substr(0, result.length()-2);
    result += ")";
    return result;
}