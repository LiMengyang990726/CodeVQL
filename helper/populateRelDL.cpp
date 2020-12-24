#include <string>
#include <fstream>
#define objectMappingDLFileName "rules/objectMapping.dl"
#define typesDLFileName "rules/types.dl"

using namespace std;

void writeObjectMappingDL() {
    ofstream objectMappingDL;
    objectMappingDL.open(objectMappingDLFileName);
    objectMappingDL << "#define Method IsMethod" << endl;
    objectMappingDL << "#define MethodAccess Call" << endl;
    objectMappingDL << "#define History history" << endl;
    objectMappingDL.close();
}

void writeTypesDL() {
    ofstream typesDL;
    typesDL.open(typesDLFileName);
    typesDL << ".type String <: symbol" << endl;
    typesDL << ".type Version <: symbol" << endl;
    typesDL << ".type Int <: unsigned" << endl;
    typesDL.close();
}

void writeAllRelDLs() {
    writeObjectMappingDL();
    writeTypesDL();
}