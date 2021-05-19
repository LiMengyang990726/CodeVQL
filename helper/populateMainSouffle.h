#ifndef POPULATE_MAIN_DL_H
#define POPULATE_MAIN_DL_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <sstream>

using namespace std;

void writeTemplate();
void writeVersion(string name);
void writeVersionComb();
void writeOutputDecl();
void writeOutput();
void writeRuleBegin();
void writeRuleTermination();
void writeNegationRule();
void writeParallelRule();
void writeEquality(string a, string b);
void writeInequality(string a, string b);
void writeArithmethics(string var, string comparision, string value);
void writeRule(string name, string field, string value);

#endif