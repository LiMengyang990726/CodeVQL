
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

    string result = ".output ";
    struct ast *varName = a->children[0];
    string varNameStr = ((struct stringval *)varName)->value;
    result += varNameStr;
    result += QLObjToDLOutput(varNameStr); // By default, output the all fields except version fields
    cout << result << endl;
    storeOutputVar(varNameStr);

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

    string result = ".decl ";
    struct ast *type = a->children[0];
    string typeStr = ((struct stringval *)type)->value;
    struct ast *name = a->children[1];
    string nameStr = ((struct stringval *)name)->value;
    result += typeStr;
    printf("%s", result);
    QLObjToDLDecl(typeStr);
    printf("\n");
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
            yyerror("error in WHERE node in AST construction");
            return;
        }
        /* TODO */
        translateWhere(a->children[0]);
        translateWhere(a->children[1]);
        break;
    case AND_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node in AST construction");
            return;
        }
        translateWhere(a->children[0]);
        printf(",");
        translateWhere(a->children[1]);
        break;
    case IF_FORMULA_NODE:
        if (a->childrencount != 3)
        {
            yyerror("error in WHERE node in AST construction");
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
            yyerror("error in WHERE node in AST construction");
            return;
        }
        /* TODO */
        translateWhere(a->children[0]);
        break;
    case COMPARISON_FORMULA_NODE:
        if (a->childrencount != 2)
        {
            yyerror("error in WHERE node in AST construction");
            return;
        }
        translateComparison(a);
        break;
    case CALL_NODE:
        if (a->childrencount < 2 || a->childrencount > 3)
        {
            yyerror("error in WHERE node in AST construction");
            return;
        }
        translateCall(a);
        break;
    }
}

void translateComparison(struct ast *a)
{
    // char *comparison = ((struct stringval *)a->m)->value;
    // struct ast *l = a->l;
    // struct ast *r = a->r;
    // if (!l || !r) {
    //   yyerror("invalid comparison syntax");
    //   return;
    // }

    // if (strcmp(comparison, "=") == 0) {
    //   if (l->nodetype == CALL_NODE && r->nodetype == STRING_NODE) {
    //     // Create a reference
    //     char *name = ((struct stringval *)l->l)->value;
    //     char *field = ((struct stringval *)l->r)->value;
    //     char referenceSymbol = printRuleReturnReference(name, field);
    //     // Store the refernce to the var
    //     char *nextName = ((struct stringval *)r)->value;
    //     storeRuleReferenceTable(referenceSymbol, nextName);
    //   }
    // } else {
    //   // I think Datalog does not provide other types of comparison like >=, <=
    // }
    // return;
}

void translateCall(struct ast *a)
{
    // // No select opts
    // if (!a) {
    //   yyerror("error in call opts");
    //   return;
    // }

    // char *name = ((struct stringval *)a->l)->value;
    // char *field = ((struct stringval *)a->m)->value;
    // if (a->r) { // For direct translate call, it can be directly translate to a rule
    //   char *value = ((struct stringval *)a->r)->value;
    //   printRule(name, field, value);
    // } // For indirect translate call, it will be handled in comparison
    // return;
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
            translateWhere(a->children[1]);
            translateSelect(a->children[2]);
            break;
        default:
            yyerror("incomplete query");
            return;
        }
    default:
        printf("internal error: bad node %c\n", a->nodetype);
    }
    return;
}
