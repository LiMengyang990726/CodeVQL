
#include <cstdarg>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <vector>
#include <sstream>
#include "utils.h"
#include "constants.h"

/* Utility for flex */
void yyerror(const char *s, ...) {
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

string constructSouffleDecl(const vector<pair<string, string> >& fieldPairs) {
    string result = "(";
    unordered_set<string> seenFields;
    for (auto & fieldPair : fieldPairs) {
        if (seenFields.find(fieldPair.first) != seenFields.end()) {
            continue;
        }
        seenFields.insert(fieldPair.first);
        result += fieldPair.first + ": " + fieldPair.second + ", ";
    }
    result = result.substr(0, result.length()-2);
    result += ")";
    return result;
}

string constructSouffleRuleBegin(const vector<string>& fields) {
    string result = "(";
    unordered_set<string> seenFields;
    for (const string& field : fields) {
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

string repeatString(const string& s, int times) {
    string result;
    while (times) {
        result += s;
        times--;
    }
    return result;
}

bool hasEnding(string const &fullString, string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool isNumber(string s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
