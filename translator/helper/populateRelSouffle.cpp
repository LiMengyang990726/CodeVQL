#include <string>
#include <fstream>
#include "symbolStore.h"
#include "constants.h"
#include "utils.h"

using namespace std;

void writeFactTypesDL() {
    ofstream factTypesDL;
    factTypesDL.open(FACT_TYPES_DL_FILE_NAME_ABS);

    // Step 1: All fact types declaration and input
    factTypesDL << ".decl TestClass" << EvoMeObjToSouffleDecl("TestClass") << endl;
    factTypesDL << ".input TestClass(filename=\"TestClass.facts\")" << endl << endl;

    factTypesDL << ".decl AbstractClass" << EvoMeObjToSouffleDecl("AbstractClass") << endl;
    factTypesDL << ".input AbstractClass(filename=\"AbstractClass.facts\")" << endl << endl;

    factTypesDL << ".decl Class" << EvoMeObjToSouffleDecl("Class") << endl;
    factTypesDL << ".input Class(filename=\"IsClass.facts\")" << endl << endl;

    factTypesDL << ".decl Method" << EvoMeObjToSouffleDecl("Method") << endl;
    factTypesDL << ".input Method(filename=\"IsMethod.facts\")" << endl << endl;

    factTypesDL << ".decl Update" << EvoMeObjToSouffleDecl("Update") << endl;
    factTypesDL << ".input Update(filename=\"UPDATE.facts\")" << endl << endl;

    factTypesDL << ".decl Insert" << EvoMeObjToSouffleDecl("Insert") << endl;
    factTypesDL << ".input Insert(filename=\"INSERT.facts\")" << endl << endl;

    factTypesDL << ".decl Delete" << EvoMeObjToSouffleDecl("Delete") << endl;
    factTypesDL << ".input Delete(filename=\"DELETE.facts\")" << endl << endl;

    factTypesDL << ".decl MethodAccess" << EvoMeObjToSouffleDecl("MethodAccess") << endl;
    factTypesDL << ".input MethodAccess(filename=\"Call.facts\")" << endl << endl;

    factTypesDL << ".decl Containment" << EvoMeObjToSouffleDecl("Containment") << endl;
    factTypesDL << ".input Containment(filename=\"Contain.facts\")" << endl << endl;

    factTypesDL << ".decl Reference" << EvoMeObjToSouffleDecl("Reference") << endl;
    factTypesDL << ".input Reference(filename=\"Reference.facts\")" << endl << endl;

    factTypesDL << ".decl Inheritance" << EvoMeObjToSouffleDecl("Inheritance") << endl;
    factTypesDL << ".input Inheritance(filename=\"Inherit.facts\")" << endl << endl;

    factTypesDL << ".decl History" << EvoMeObjToSouffleDecl("History") << endl;
    factTypesDL << ".input History(filename=\"history.facts\")" << endl << endl;

    // Step 2: EvoMe unique relations
    //      Type 1: Reachable
    factTypesDL << ".decl Reachable(a:Version, b:Version)" << endl;
    factTypesDL << "Reachable(a, b) :- History(a, b, _)." << endl;
    factTypesDL << "Reachable(a, x) :- History(a, b, _), Reachable(b, x)." << endl;
    factTypesDL << "Reachable(a, y) :- History(x, y, _), Reachable(a, x)." << endl;
    factTypesDL << "Reachable(a, a) :- History(a, _, _); History(_, a, _)." << endl
                << endl;
    //      Type 2: CommitDist
    factTypesDL << ".decl CommitDist(child: Version, parent: Version, index: Int)" << endl;
    factTypesDL << "CommitDist(a, x, n+1) :- CommitDist(a, y, n), History(y, x, 0)." << endl
                << endl;

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