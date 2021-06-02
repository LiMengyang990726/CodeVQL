
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "nodetype.h"
#include "symbolStore.h"
#include "populateMainSouffle.h"
#include "populateRelSouffle.h"
#include "populateVersionSouffle.h"
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

void translateRange(struct ast *a, vector<string>& versions)
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
    if (!isVersionDeclared(versionNameStr)) {
        yyerror("version is not defined");
        return;
    }
    storeVersionVarAssociationTable(versionNameStr, varNameStr);
    versions.push_back(versionNameStr);

    if (a->childrencount == 2)
    {
        writeVersionsCombination(versions);
        return;
    }
    else
    {
        translateRange(a->children[2], versions);
        return;
    }
}

void translateReasonOpts(struct ast *a, vector<string>& reasonOpts)
{
    // No opts multiple selections, reject
    if (!a || (a->childrencount < 1) || (a->childrencount > 2))
    {
        yyerror("Error in reasoning opts");
    }

    struct ast *varName = a->children[0];
    string varNameStr = ((struct stringval *)varName)->value;
    string versionNameStr = findVersionVarAssociation(varNameStr);
    if (versionNameStr == "") {
        yyerror("The current variable has not declared its selected versions");
    }
    reasonOpts.push_back(varNameStr);

    if (a->childrencount == 1)
    {
        return;
    }
    else
    {
        translateReasonOpts(a->children[1], reasonOpts);
        return;
    }
}

void translateWhere(struct ast *a) 
{
    // No where opts, accept
    if (!a)
    {
        return;
    } else if (a->childrencount < 3) {
        yyerror("error in WHERE statement: incorrect number of child when parsing WHERE to an abstract syntax tree");
        return;
    }

    struct ast* reason = a->children[0];
    string reasonStr = ((struct stringval *)reason)->value;
    vector<string> reasonOpts;
    translateReasonOpts(a->children[1], reasonOpts); // check if all versions used here are definied & get them
    if (reasonStr == "exists") {
        translateFormula(a->children[2]);
    } else if (reasonStr == "not exist") {
        for (string reasonOpt : reasonOpts) {
            storenotExistSpecifiedVarsSet(reasonOpt);
        }
        translateFormula(a->children[2]);
    } else if (reasonStr == "forall") {
        translateFormula(a->children[2]);
    } else {
        yyerror("error in WHERE reasoning statement: no such reasoning keyword defined");
        return;
    }

    if (a->childrencount == 3) {
        return;
    } else {
        writeParallelRule(); // between where clauses, can only be "AND" for now
        clearnotExistSpecifiedVarsSet();
        translateWhere(a->children[3]);
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
    if (findVarDeclaration(nameStr) != "Version") {
        string fieldStr;
        if (name->nodetype == CALL_NODE) {
            // Select result is CALL
            nameStr = ((struct stringval *)name->children[0])->value;
            fieldStr = ((struct stringval *)name->children[1])->value;
        } else {
            // Select result is LOWER_ID
            string type = findVarDeclaration(nameStr);
            fieldStr = getIdName(type);
        }
        storeVarFieldReferenceTable(nameStr + "." + fieldStr, fieldStr + nameStr);
        storeOutputVarField(nameStr, fieldStr);
    }

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

void translateFormula(struct ast *a, bool negated)
{
    // No where opts, accept
    if (!a)
    {
        return;
    }

    switch (a->nodetype)
    {
    case OR_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node (OR) in AST construction");
            return;
        }
        translateFormula(a->children[0]);
        writeOrRule();
        translateFormula(a->children[1]);
        break;
    case IMPLIES_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node (IMPLIES) in AST construction");
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
        translateFormula(a->children[0], true);
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
        translateCall(a, negated);
        break;
    case PRECEDANCE_FORMULA_NODE:
        if (a->childrencount != 1)
        {
            yyerror("error in WHERE node (PRECEDANCE) in AST construction");
            return;
        }
        writeLeftBracket();
        translateFormula(a->children[0]);
        writeRightBracket();
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

    if (comparison == "=" || comparison == "!=")
    {
        if (l->nodetype == CALL_NODE && r->nodetype == STRING_NODE)
        {
            string nameStr = ((struct stringval *)l->children[0])->value;
            string fieldStr = ((struct stringval *)l->children[1])->value;
            string nextName = ((struct stringval *)r)->value;
            int nextNameLen = nextName.length();
            if (nextNameLen >= 2 && nextName[0] == '\"' && nextName[nextName.length()-1] == '\"') {
                // Case 1: xxx.xx() = "..."
                if (comparison == "!=") {
                    writeNegationRule();
                }
                if (nextNameLen == 3 && nextName[1] == '*') {
                    // Case 1.1: Handle "..." is wildcard
                    writeRule(nameStr, fieldStr, "_");
                } else {
                    writeRule(nameStr, fieldStr, nextName);
                }
            } else if (isNumber(nextName)) {
                // Case 2: xxx.xx() = number
                string referer = fieldStr + nameStr;
                storeVarFieldReferenceTable(nameStr + "." + fieldStr, referer);
                writeRule(nameStr, fieldStr, referer);
                writeParallelRule();
                writeArithmethics(referer, comparison, nextName);
            } else {
                // Case 3: xxx.xx() = lower_id
                // Check 1: If left side is closure
                if (isClosureMethod(fieldStr)) {
                    string type = findVarDeclaration(nameStr);
                    if (!isTypeValidClosure(type)) {
                        yyerror("Invalid type for writing closure");
                        return;
                    }
                    writeClosure(type);
                }
                string nextNameIdName = getIdName(findVarDeclaration(nextName));
                if (findVarFieldReferredName(nextName, nextNameIdName) != "") {
                    // Case 3.1: lower_id has been referred before
                    string referer = findVarFieldReferredName(nextName, nextNameIdName);
                    if (findVarFieldReferredName(nameStr, fieldStr) != "") {
                        // Case 3.1.1: xxx.xx() also have been referred before
                        string prevReferer = findVarFieldReferredName(nameStr, fieldStr);
                        if (comparison == "!=") {
                            writeInequality(prevReferer, referer);
                        } else {
                            writeEquality(prevReferer, referer);
                        }
                    } else {
                        // Case 3.1.2: xxx.xx() is not referer before
                        storeVarFieldReferenceTable(nameStr + "." + fieldStr, referer);
                        if (comparison == "!=") {
                            writeNegationRule();
                        }
                        writeRule(nameStr, fieldStr, referer);
                        writeParallelRule();
                        writeRule(nextName, nextNameIdName, referer);
                    }
                } else if (findVarFieldReferredName(nameStr, fieldStr) != "") {
                    // Case 3.2: lower_id has NOT been referred before, xxx.xx() has been referred before
                    string prevReferer = findVarFieldReferredName(nameStr, fieldStr);
                    storeVarFieldReferenceTable(nextName + "." + nextNameIdName, prevReferer);
                    if (comparison == "!=") {
                        writeNegationRule();
                    }
                    writeRule(nameStr, fieldStr, prevReferer);
                    writeParallelRule();
                    writeRule(nextName, nextNameIdName, prevReferer);
                } else {
                    // Case 3.3: lower_id and xxx.xx() have NOT been referrer before
                    string referer = "";
                    if (isClosureMethod(fieldStr)) {
                        referer = fieldStr.substr(0, fieldStr.length()-1) + nameStr;
                    } else {
                        referer = fieldStr + nameStr;
                    }
                    storeVarFieldReferenceTable(nameStr + "." + fieldStr, referer);
                    writeRule(nameStr, fieldStr, referer);
                    writeParallelRule();
                    if (comparison == "!=") {
                        storeVarFieldReferenceTable(nextName + "." + nextNameIdName, "!" + referer); 
                        writeRule(nextName, nextNameIdName, "!" + referer);
                    } else {
                        storeVarFieldReferenceTable(nextName + "." + nextNameIdName, referer);
                        writeRule(nextName, nextNameIdName, referer);
                    }
                }
            }
        } 
        else if (l->nodetype == CALL_NODE && r->nodetype == CALL_NODE) 
        {
            string lNameStr = ((struct stringval *)l->children[0])->value;
            string lFieldStr = ((struct stringval *)l->children[1])->value;
            string rNameStr = ((struct stringval *)r->children[0])->value;
            string rFieldStr = ((struct stringval *)r->children[1])->value;
            if (isDirectFieldExtraction(lNameStr, lFieldStr) && isDirectFieldExtraction(rNameStr, rFieldStr)) {
                // Case 1: Field comparison
                string referer = rFieldStr + rNameStr; // Set the referer to the right side
                storeVarFieldReferenceTable(rNameStr + "." + rFieldStr, referer);
                writeRule(rNameStr, rFieldStr, referer);
                writeParallelRule();
                if (comparison == "!=") { 
                    storeVarFieldReferenceTable(lNameStr + "." + lFieldStr, "!" + referer); 
                    writeRule(lNameStr, lFieldStr, "!" + referer);  
                } else {
                    storeVarFieldReferenceTable(lNameStr + "." + lFieldStr, referer);
                    writeRule(lNameStr, lFieldStr, referer);
                }
            } else {
                // Case 2: CodeVQL unique object methods
                if (isClosureMethod(lFieldStr) && isClosureMethod(rFieldStr)) {
                    // Case 2.1: Both sides are closure
                    if (!isTypeValidClosure(findVarDeclaration(lNameStr)) || !isTypeValidClosure(findVarDeclaration(rNameStr))) {
                        yyerror("Invalid type for writing closure");
                        return;
                    }
                    writeClosure(findVarDeclaration(lNameStr));
                    writeClosure(findVarDeclaration(rNameStr));
                    string referer = rFieldStr.substr(0, rFieldStr.length()-1) + rNameStr; // Remove special char
                    storeVarFieldReferenceTable(rNameStr + "." + rFieldStr, referer);
                    writeRule(rNameStr, rFieldStr, referer); // Need special one for writing the closure
                    writeParallelRule();
                    if (comparison == "!=") {
                        storeVarFieldReferenceTable(lNameStr + "." + lFieldStr, "!" + referer);
                        writeRule(lNameStr, lFieldStr, referer);
                    } else {
                        storeVarFieldReferenceTable(lNameStr + "." + lFieldStr, referer);
                        writeRule(lNameStr, lFieldStr, referer);            
                    }
                } else if (isClosureMethod(lFieldStr)) {
                    // Case 2.2: Left side is closure, right side is a normal call
                    // TODO
                } else if (isClosureMethod(rFieldStr)) {
                    // Case 2.3 Right side is closure, left side is a normal call
                    // TODO 
                }
            }
        } 
        else if (l->nodetype == STRING_NODE && r->nodetype == STRING_NODE) 
        {
            // Assume l and r are two LOWER_ID. TODO: For other cases
            string lStr = ((struct stringval *)l)->value;
            string rStr = ((struct stringval *)r)->value;
            string lTypeId = getIdName(findVarDeclaration(lStr));
            string rTypeId = getIdName(findVarDeclaration(rStr));
            storeVarFieldReferenceTable(lStr + "." + lTypeId, lTypeId + lStr);
            storeVarFieldReferenceTable(rStr + "." + rTypeId, rTypeId + rStr);
            if (comparison != "=") {
                writeInequality(lTypeId + lStr, rTypeId + rStr);
            } else {
                writeEquality(lTypeId + lStr, rTypeId + rStr);
            }
        }
    }
    else if (comparison == ">=" || comparison == "<=" || comparison == ">" || comparison == "<")
    {
        if (l->nodetype == CALL_NODE && r->nodetype == STRING_NODE)
        {
            // Hanle only the case: xxx.xx() comp number
            string nameStr = ((struct stringval *)l->children[0])->value;
            string fieldStr = ((struct stringval *)l->children[1])->value;
            string value = ((struct stringval *)r)->value;
            string referer = fieldStr + nameStr;
            storeVarFieldReferenceTable(nameStr + "." + fieldStr, referer);
            writeRule(nameStr, fieldStr, referer);
            writeParallelRule();
            writeArithmethics(referer, comparison, value);
        }
        // TODO: Handle these comparisons for other types of l & r nodes
    }
    else 
    {
        // TODO: Handle other types of comparisons when seen
    }
    return;
}

void translateCall(struct ast *a, bool negated)
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
    { 
        // For direct translate call, it can be directly translate to a rule
        char *value = ((struct stringval *)a->children[2])->value;
        if (negated)
        {
            writeNegationRule();
        }
        writeRule(nameStr, fieldStr, value);
    } else if (a->childrencount == 2) {
        // For indirect translate call, it is a boolean value
        writeRule(nameStr, fieldStr, "\"true\"");
    }
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
    vector<string> versions;
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
            translateRange(a->children[1], versions);
            translateSelect(a->children[3]);
            writeOutputDecl();
            writeRuleBegin();
            translateWhere(a->children[2]);
            writeParallelRule();
            writeVersionCombInRule();
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
