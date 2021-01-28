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
void writeDecl(string type);
void writeOutputDecl();
void writeInput(string type);
void writeOutput();
void writeRuleBegin();
void writeRuleTermination();
void writeParallelRule();
void writeArithmethics(string var, string comparision, string value);
void writeRule(string name, string field, string value);

#endif