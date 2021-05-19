#include <string>
#include <fstream>
#include "symbolStore.h"
#include "constants.h"

using namespace std;

void writeFactTypesDL() {
    ofstream factTypesDL;
    factTypesDL.open(FACT_TYPES_DL_FILE_NAME_ABS);

    // Step 1: All fact types declaration and input
    factTypesDL << ".decl TestClass" << CodeVQLObjToSouffleDecl("TestClass") << endl;
    factTypesDL << ".input TestClass(filename=\"TestClass.facts\")" << endl << endl;

    factTypesDL << ".decl AbstractClass" << CodeVQLObjToSouffleDecl("AbstractClass") << endl;
    factTypesDL << ".input AbstractClass(filename=\"AbstractClass.facts\")" << endl << endl;

    factTypesDL << ".decl Method" << CodeVQLObjToSouffleDecl("Method") << endl;
    factTypesDL << ".input Method(filename=\"Method.facts\")" << endl << endl;

    factTypesDL << ".decl Update" << CodeVQLObjToSouffleDecl("Update") << endl;
    factTypesDL << ".input Update(filename=\"UPDATE.facts\")" << endl << endl;

    factTypesDL << ".decl Insert" << CodeVQLObjToSouffleDecl("Insert") << endl;
    factTypesDL << ".input Insert(filename=\"INSERT.facts\")" << endl << endl;

    factTypesDL << ".decl Delete" << CodeVQLObjToSouffleDecl("Delete") << endl;
    factTypesDL << ".input Delete(filename=\"DELETE.facts\")" << endl << endl;

    factTypesDL << ".decl MethodAccess" << CodeVQLObjToSouffleDecl("MethodAccess") << endl;
    factTypesDL << ".input MethodAccess(filename=\"Call.facts\")" << endl << endl;

    factTypesDL << ".decl Containment" << CodeVQLObjToSouffleDecl("Containment") << endl;
    factTypesDL << ".input Containment(filename=\"Contain.facts\")" << endl << endl;

    factTypesDL << ".decl Reference" << CodeVQLObjToSouffleDecl("Reference") << endl;
    factTypesDL << ".input Reference(filename=\"Reference.facts\")" << endl << endl;

    factTypesDL << ".decl Inheritance" << CodeVQLObjToSouffleDecl("Inheritance") << endl;
    factTypesDL << ".input Inheritance(filename=\"Inherit.facts\")" << endl << endl;

    factTypesDL << ".decl History" << CodeVQLObjToSouffleDecl("History") << endl;
    factTypesDL << ".input History(filename=\"history.facts\")" << endl << endl;

    // Step 2: CodeVQL unique relations
    //         Type 1: getClosure()
    factTypesDL << ".decl DependOn(a: String, b: String)" << endl;
    factTypesDL << "DependOn(x, y) :- MethodAccess(x, y, _)." << endl;
    factTypesDL << "DependOn(x, y) :- Containment(x, y, _)." << endl;
    factTypesDL << "DependOn(x, y) :- Reference(x, y, _)." << endl;
    factTypesDL << "DependOn(x, y) :- Inheritance(x, y, _)" << endl;
    factTypesDL << "Closure(x, y) :- DependOn(x, y)." << endl;
    factTypesDL << "Closure(x, y) :- DependOn(x, m), DependOn(m, y)." << endl << endl;
    //         TODO: get other closures

    factTypesDL.close();
}

void writeBaseTypesDL() {
    ofstream typesDL;
    typesDL.open(BASE_TYPES_DL_FILE_NAME_ABS);
    typesDL << ".type String <: symbol" << endl;
    typesDL << ".type Version <: symbol" << endl;
    typesDL << ".type Int <: unsigned" << endl;
    typesDL.close();
}

void writeAllRelDLs() {
    writeFactTypesDL();
    writeBaseTypesDL();
}