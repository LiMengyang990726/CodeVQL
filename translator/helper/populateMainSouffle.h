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
void writeVersion(const string& name);
void writeVersionComb();
void writeVersionCombInRule();
void writeOutputDecl();
void writeOutput();
void writeRuleBegin();
void writeRuleTermination();
void writeNegationRule();
void writeParallelRule();
void writeOrRule();
void writeLeftBracket();
void writeRightBracket();
void writeEquality(const string& a, const string& b);
void writeInequality(const string& a, const string& b);
void writeArithmethics(const string& var, const string& comparision, const string& value);
void writeIncludeEvoMeUniqueMethod();
void writeClosure(const string& type);
void writeRule(const string& name, string field, string value);

#endif