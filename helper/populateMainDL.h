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
void writeDecl(string type);
void writeOutputDecl(string type);
void writeInput(string type);
void writeOutput();
void writeRuleBegin();
void writeRuleTermination();
void writeParallelRule();
void writeRule(string name, string field, string value);

#endif