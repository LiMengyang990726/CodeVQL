
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "nodetype.h"
#include "symbolStore.h"
#include "populateMainDL.h"
#include "populateRelDL.h"
#include "populateVersionDL.h"
#include "utils.h"
#include "ASTNode.h"
#include "translate.h"
using namespace std;

/* Translations for AST */
void translateImportStmt(struct ast *a)
{
    // No import opts, reject
    if (!a || a->childrencount != 1)
    {
        yyerror("error in import statement");
        return;
    }

    struct ast *importOpts = a->children[0];
    if (strcmp(((struct stringval *)importOpts)->value, "java"))
    {
        yyerror("only java is supported");
        return;
    }
}

void translateDefineStmt(struct ast *a)
{
    // No define opts, reject
    if (!a || a->childrencount != 1)
    {
        yyerror("error in define statement");
        return;
    } 

    translateDefineOpts(a->children[0]);
    return;
}

vector<string> translateMultipleVersionType1Opts(struct ast *a, vector<string> &prev)
{
    // No opts multiple selections, reject
    if (!a || (a->childrencount < 1) || (a->childrencount > 2))

    {
        yyerror("Error in multiple version selection");
        return prev;
    }

    struct ast *range = a->children[0];
    string rangeStr = ((struct stringval *)range)->value;
    prev.push_back(rangeStr);

    if (a->childrencount == 1)
    {
        return prev;
    }
    else
    {
        translateMultipleVersionType1Opts(a->children[1], prev);
        return prev;
    }
}

void translateDefineOpts(struct ast *a)
{
    if (!a) {
        yyerror("error in define statement opts");
        return;
    }

    if (a->nodetype == SINGLE_VERSION_NODE)
    {
        // Check if children count is valid
        if (a->childrencount < 2 || a->childrencount > 3)
        {
            yyerror("error in define statement AST construction: children count not supported in single version selection");
            return;
        }

        // Logic for single version translation
        struct ast *name = a->children[0];
        string nameStr = ((struct stringval *)name)->value;
        struct ast *range = a->children[1];
        string rangeStr = ((struct stringval *)range)->value;
        storeVarDeclarationTable(TYPE_VERISON, nameStr);
        storeVersionDeclarationTable(nameStr);
        writeVersion(nameStr);
        writeVersionDL(nameStr, rangeStr);

        // Do the recursion
        if (a->childrencount == 2)
        {
            return;
        }
        else
        {
            translateDefineOpts(a->children[2]);
            return;
        }
    }
    else if (a->nodetype == MULTIPLE_VERSIONS_TYPE_1_NODE)
    {
        // Check if children count is valid
        if (a->childrencount < 2 || a->childrencount > 3)
        {
            yyerror("error in define statement AST construction: children count not supported in multiple version selection");
            return;
        }

        // Logic for multiple versions translation
        struct ast *name = a->children[0];
        string nameStr = ((struct stringval *)name)->value;
        struct ast *range = a->children[1];
        vector<string> rangeStrs;
        rangeStrs = translateMultipleVersionType1Opts(range, rangeStrs);
        storeVarDeclarationTable(TYPE_VERISON, nameStr);
        storeVersionDeclarationTable(nameStr);
        writeVersion(nameStr);
        writeVersionDL(nameStr, MULTIPLE_VERSIONS_TYPE_1, rangeStrs);

        // Do the recursion
        if (a->childrencount == 2)
        {
            return;
        }
        else
        {
            translateDefineOpts(a->children[2]);
            return;
        }
    }
    else if (a->nodetype == MULTIPLE_VERSIONS_TYPE_2_NODE)
    {
        // Check if children count is valid
        if (a->childrencount < 3 || a->childrencount > 4)
        {
            yyerror("error in define statement AST construction: children count not supported in single version selection");
            return;
        }

        struct ast *name = a->children[0];
        string nameStr = ((struct stringval *)name)->value;
        struct ast *fromCommit = a->children[1];
        string fromCommitStr = ((struct stringval *)fromCommit)->value;
        struct ast *baseCommit = a->children[2];
        string baseCommitStr = ((struct stringval *)baseCommit)->value;
        vector<string> rangeStrs;
        rangeStrs.push_back(fromCommitStr);
        rangeStrs.push_back(baseCommitStr);
        storeVarDeclarationTable(TYPE_VERISON, nameStr);
        storeVersionDeclarationTable(nameStr);
        writeVersion(nameStr);
        writeVersionDL(nameStr, MULTIPLE_VERSIONS_TYPE_2, rangeStrs);

        // Do the recursion
        if (a->childrencount == 3)
        {
            return;
        }
        else
        {
            translateDefineOpts(a->children[3]);
            return;
        }
    }
    else
    {
        yyerror("error in define statement AST construction: no such version selection method");
        return;
    }
}

void translateFrom(struct ast *a)
{
    // No fromt opts, accept
    if (!a)
    {
        return;
    }
    else if (a->childrencount < 2 || a->childrencount > 3)
    {
        yyerror("error in FROM node in AST construction");
        return;
    }

    struct ast *type = a->children[0];
    string typeStr = ((struct stringval *)type)->value;
    struct ast *name = a->children[1];
    string nameStr = ((struct stringval *)name)->value;
    writeDecl(typeStr);
    writeInput(typeStr);
    storeVarDeclarationTable(typeStr, nameStr);

    if (a->childrencount == 2)
    {
        return;
    }
    else
    {
        translateFrom(a->children[2]);
        return;
    }
}

void translateRange(struct ast *a)
{
    // No range opts, accept
    if (!a)
    {
        return;
    } else if (a->childrencount < 2 || a->childrencount > 3) {
        yyerror("error in range clause");
        return;
    }

    struct ast *varName = a->children[0];
    string varNameStr = ((struct stringval *)varName)->value;
    struct ast *versionName = a->children[1];
    string versionNameStr = ((struct stringval *)versionName)->value;
    if (!findVersionDeclaration(versionNameStr)) {
        yyerror("version is not defined");
        return;
    }
    storeVersionVarAssociationTable(versionNameStr, varNameStr);

    if (a->childrencount == 2)
    {
        return;
    }
    else
    {
        translateRange(a->children[2]);
        return;
    }
}

vector<string> translateReasonOpts(struct ast *a, vector<string> &prev)
{
    // No opts multiple selections, reject
    if (!a || (a->childrencount < 1) || (a->childrencount > 2))

    {
        yyerror("Error in reasoning opts");
        return prev;
    }

    struct ast *varName = a->children[0];
    string varNameStr = ((struct stringval *)varName)->value;
    string versionNameStr = findVersionVarAssociation(varNameStr);
    if (versionNameStr == "") {
        yyerror("The current variable has not declared its selected versions");
        return prev;
    }
    prev.push_back(versionNameStr);

    if (a->childrencount == 1)
    {
        return prev;
    }
    else
    {
        translateReasonOpts(a->children[1], prev);
        return prev;
    }
}

void translateWhere(struct ast *a) 
{
    // No where opts, accept
    if (!a)
    {
        return;
    } else if (a->childrencount != 3) {
        yyerror("error in WHERE statement");
        return;
    }

    struct ast* reason = a->children[0];
    string reasonStr = ((struct stringval *)reason)->value;
    if (reasonStr == "exists") {
        vector<string> varNames;
        writeVersionsCombination(translateReasonOpts(a->children[1], varNames));
        translateFormula(a->children[2]);
    } else if (reasonStr == "forall") {
        translateFormula(a->children[2]);
    } else {
        yyerror("error in WHERE reasoning statement");
        return;
    }
}

void translateSelect(struct ast *a)
{
    // No select opts
    if (!a)
    {
        yyerror("error in select opts");
        return;
    }
    else if (a->childrencount < 1 || a->childrencount > 2)
    {
        yyerror("error in SELECT node in AST construction");
        return;
    }

    struct ast *name = a->children[0];
    string nameStr = ((struct stringval *)name)->value;
    storeOutputVar(nameStr);

    if (a->childrencount == 1)
    {
        return;
    }
    else
    {
        translateSelect(a->children[1]);
        return;
    }
}

void translateFormula(struct ast *a)
{
    // No where opts, accept
    if (!a)
    {
        return;
    }

    switch (a->nodetype)
    {
    case OR_FORMULA_NODE:
    case IMPLIES_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node (OR/IMPLIES) in AST construction");
            return;
        }
        /* TODO */
        translateFormula(a->children[0]);
        translateFormula(a->children[1]);
        break;
    case AND_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node (AND) in AST construction");
            return;
        }
        translateFormula(a->children[0]);
        writeParallelRule();
        translateFormula(a->children[1]);
        break;
    case IF_FORMULA_NODE:
        if (a->childrencount != 3)
        {
            yyerror("error in WHERE node (IF) in AST construction");
            return;
        }
        /* TODO */
        translateFormula(a->children[0]);
        translateFormula(a->children[1]);
        translateFormula(a->children[2]);
        break;
    case NOT_FORMULA_NODE:
        if (a->childrencount != 1)
        {
            yyerror("error in WHERE node (NOT) in AST construction");
            return;
        }
        /* TODO */
        translateFormula(a->children[0]);
        break;
    case COMPARISON_FORMULA_NODE:
        if (a->childrencount != 3)
        {
            yyerror("error in WHERE node (COMPARISON) in AST construction");
            return;
        }
        translateComparison(a);
        break;
    case CALL_NODE:
        if (a->childrencount < 2 || a->childrencount > 3)
        {
            yyerror("error in WHERE node (CALL) in AST construction");
            return;
        }
        translateCall(a);
        break;
    }
}

void translateComparison(struct ast *a)
{
    // No where opts, accept
    if (!a || a->childrencount != 3)
    {
        yyerror("error in constructing where comparison");
        return;
    }

    string comparison = ((struct stringval *)a->children[1])->value;
    struct ast *l = a->children[0];
    struct ast *r = a->children[2];

    if (comparison == "=")
    {
        if (l->nodetype == CALL_NODE && r->nodetype == STRING_NODE)
        {
            string nameStr = ((struct stringval *)l->children[0])->value;
            string fieldStr = ((struct stringval *)l->children[1])->value;
            writeRule(nameStr, fieldStr, "");

            // Create a reference
            string nextName = ((struct stringval *)r)->value;
            storeRuleReferenceTable(fieldStr, nextName);
        }
    }
    else
    {
        // TODO: For other comparison types like >=, >, etc.
    }
    return;
}

void translateCall(struct ast *a)
{
    // No select opts
    if (!a || a->childrencount < 2 || a->childrencount > 3)
    {
        yyerror("error in constructing function call");
        return;
    }

    string nameStr = ((struct stringval *)a->children[0])->value;
    string fieldStr = ((struct stringval *)a->children[1])->value;
    if (a->childrencount == 3)
    { // For direct translate call, it can be directly translate to a rule
        char *value = ((struct stringval *)a->children[2])->value;
        writeRule(nameStr, fieldStr, value);
    } // For indirect translate call, it will be handled in comparison
    return;
}

void eval(struct ast *a)
{
    if (!a)
    {
        yyerror("internal error, null eval");
        return;
    }

    writeAllRelDLs();
    writeTemplate();
    switch (a->nodetype)
    {
    /* import statement */
    case IMPORT_STMT_NODE:
        translateImportStmt(a);
        break;
    case DEFINE_STMT_NODE:
        translateDefineStmt(a);
        break;
    /* select statement */
    case SELECT_STMT_NODE:
        switch (a->childrencount)
        {
        case 1:
            translateSelect(a->children[0]);
            break;
        case 2:
            translateFrom(a->children[0]);
            translateSelect(a->children[1]);
            break;
        case 3:
            translateFrom(a->children[0]);
            translateSelect(a->children[2]);
            writeOutputDecl();
            writeRuleBegin();
            translateWhere(a->children[1]);
            writeRuleTermination();
            break;
        case 4:
            translateFrom(a->children[0]);
            translateRange(a->children[1]);
            translateSelect(a->children[3]);
            writeOutputDecl();
            writeRuleBegin();
            translateWhere(a->children[2]);
            writeRuleTermination();
            writeVersionComb();
            break;
        default:
            yyerror("incomplete query");
            return;
        }
        writeOutput();
        break;
    default:
        yyerror("internal error: bad node %c\n", a->nodetype);
        return;
    }
    return;
}
