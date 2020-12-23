
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "nodetype.h"
#include "symbolStore.h"
#include "printUtils.h"
#include "utils.h"
#include "ASTNode.h"
#include "translate.h"
using namespace std;

/* Translations for AST */
void translateImportStmt(struct ast *a)
{
    // No import opts
    if (!a || a->childrencount != 1)
    {
        yyerror("no import language");
        return;
    }

    struct ast *importOpts = a->children[0];
    if (strcmp(((struct stringval *)importOpts)->value, "java"))
    {
        yyerror("only java is supported");
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
    string typeStr = findVarDeclaration(nameStr);
    storeOutputVar(nameStr);
    printOutputDecl(typeStr);

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
    printDecl(typeStr);
    printInput(typeStr);
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

void translateRange(struct ast *a) {
    // No range opts, accept
    if (!a) {
        return;
    } else if (a->childrencount < 2 || a->childrencount > 3) {
        yyerror("error in RANGE node in AST construction");
        return;
    }

    struct ast *name = a->children[0];
    string nameStr = ((struct stringval *)name)->value;
    struct ast *range = a->children[1];
    string rangeStr = ((struct stringval *)range)->value;
    if (findVarDeclaration(nameStr).empty()) {
        yyerror("The variable in RANGE clause is not declared");
        return;
    }
    storeVarRangeTable(nameStr, rangeStr);

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

void translateWhere(struct ast *a)
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
        translateWhere(a->children[0]);
        translateWhere(a->children[1]);
        break;
    case AND_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node (AND) in AST construction");
            return;
        }
        translateWhere(a->children[0]);
        printf(",");
        translateWhere(a->children[1]);
        break;
    case IF_FORMULA_NODE:
        if (a->childrencount != 3)
        {
            yyerror("error in WHERE node (IF) in AST construction");
            return;
        }
        /* TODO */
        translateWhere(a->children[0]);
        translateWhere(a->children[1]);
        translateWhere(a->children[2]);
        break;
    case NOT_FORMULA_NODE:
        if (a->childrencount != 1)
        {
            yyerror("error in WHERE node (NOT) in AST construction");
            return;
        }
        /* TODO */
        translateWhere(a->children[0]);
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

    if (comparison == "=") {
      if (l->nodetype == CALL_NODE && r->nodetype == STRING_NODE) {
        string nameStr = ((struct stringval *)l->children[0])->value;
        string fieldStr = ((struct stringval *)l->children[1])->value;
        printRule(nameStr, fieldStr, "");

        // Create a reference
        string nextName = ((struct stringval *)r)->value;
        storeRuleReferenceTable(fieldStr, nextName);
      }
    } else {
      // TODO: For other comparison types like >=, >, etc.
    }
    return;
}

void translateCall(struct ast *a)
{
    // No select opts
    if (!a || a->childrencount < 2 || a->childrencount > 3) {
      yyerror("error in constructing function call");
      return;
    }

    string nameStr = ((struct stringval *)a->children[0])->value;
    string fieldStr = ((struct stringval *)a->children[1])->value;
    if (a->childrencount == 3) { // For direct translate call, it can be directly translate to a rule
      char *value = ((struct stringval *)a->children[2])->value;
      printRule(nameStr, fieldStr, value);
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

    switch (a->nodetype)
    {
    /* import statement */
    case IMPORT_STMT_NODE:
        translateImportStmt(a);
        break;
    /* select statement */
    case SELECT_STMT_NODE:
        printTemplate();
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
            printRuleBegin();
            translateWhere(a->children[1]);
            printRuleTermination();
            break;
        case 4: 
            translateFrom(a->children[0]);
            translateRange(a->children[1]);
            translateSelect(a->children[3]);
            printRuleBegin();
            translateWhere(a->children[2]);
            printRuleTermination();
            break;
        default:
            yyerror("incomplete query");
            return;
        }
        printOutput();
        break;
    default:
        printf("internal error: bad node %c\n", a->nodetype);
        return;
    }
    return;
}
