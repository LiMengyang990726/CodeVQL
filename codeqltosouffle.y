/* Parser for codeql subset */
%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "symbolTable.cpp"
#include "utility.cpp"
#include "ast/ASTNode.h"
  
/* from the lexer */
extern int yylex();
void yyerror(char *s, ...);

/* nodes in the abstract syntax tree */
struct ast {
  int nodetype;
  struct ast *l;
  struct ast *m;
  struct ast *r;
};
struct stringval {
  int nodetype;
  char *value;
};

/* methods for constructing nodes */
struct ast *newast_1(int nodetype, struct ast *l, struct ast *m, struct ast *r);
struct ast *newast_2(int nodetype, char *valueL, char *valueM, struct ast *r);
struct ast *newast_3(int nodetype, char *valueL, struct ast *m, char *valueR);
struct ast *newast_4(int nodetype, char *valueL, char *valueM, char *valueR);
struct ast *newstringval(char *value);

/* evaluate an AST */
void translateImportStmt(struct ast *);
void translateFrom(struct ast *);
void translateWhere(struct ast *);
void translateSelect(struct ast *);
void translateCall(struct ast *);
void translateComparison(struct ast *);
void eval(struct ast *);
%}

%union {
        struct ast *a;
        char *strval;
        char *subtok;
        int intval;
}

%token <strval> IMPORT
%token <strval> JAVA
%token <strval> GO
%token <strval> CSHARP
%token <strval> CPP
%token <strval> PYTHON
%token <strval> JAVASCRIPT
%token <strval> FROM
%token <strval> WHERE
%token <strval> OR
%token <strval> AND
%token <strval> IMPLIES
%token <strval> IF
%token <strval> ELSE
%token <strval> THEN
%token <strval> NOT
%token <strval> SELECT
%token <strval> AS
%token <strval> STRING_LITERAL
%token <strval> UPPER_ID
%token <strval> LOWER_ID
%token <strval> COMMA
%token <strval> LEFT_BRACKET
%token <strval> RIGHT_BRACKET
%token <strval> DOT
%token <strval> UNDERSCORE
%token <subtok> COMPARISON

%type <a> stmt import_stmt select_stmt
%type <a> select_opts from_opts where_opts 
%type <a> formula call primary expr
%type <strval> import_opts   

%start stmt_list

%%

stmt_list: stmt { eval($1); }
  | stmt_list stmt { eval($2); }
  ;

stmt: import_stmt { $$ = $1; }
  |   select_stmt { $$ = $1; }
  ;

import_stmt: IMPORT import_opts { $$ = newast_3(1, $1, NULL, $2); } ;
import_opts: JAVA { $$ = "java"; }
  | GO { $$ = "go"; }
  | CSHARP { $$ = "c#"; }
  | CPP  { $$ = "cpp"; }
  | PYTHON { $$ = "python"; }
  | JAVASCRIPT { $$ = "javscript"; }
  ;

select_stmt: SELECT select_opts { $$ = newast_1(2, NULL, NULL, $2); }
  | FROM from_opts SELECT select_opts  { $$ = newast_1(2, $2, NULL, $4); }
  | FROM from_opts WHERE where_opts SELECT select_opts { $$ = newast_1(2, $2, $4, $6); }
  ;

select_opts: expr { $$ = newast_1(3, $1, NULL, NULL); }
  | expr AS LOWER_ID { $$ = newast_2(3, $3, NULL, NULL);  }
  | select_opts COMMA expr { $$ = newast_1(3, $3, NULL, $1); }
  | select_opts COMMA expr AS LOWER_ID  { $$ = newast_2(3, $5, NULL, $1); }
  ;

from_opts: UPPER_ID LOWER_ID { $$ = newast_2(4, $1, $2, NULL); }
  | from_opts COMMA UPPER_ID LOWER_ID { $$ = newast_2(4, $3, $4, $1); }
  ;

where_opts: formula { $$ = $1; };
formula: LEFT_BRACKET formula RIGHT_BRACKET { $$ = $2; }
  |   formula OR formula { $$ = newast_1(5, $1, NULL, $3); }
  |   formula AND formula { $$ = newast_1(6, $1, NULL, $3); }
  |   formula IMPLIES formula { $$ = newast_1(7, $1, NULL, $3); }
  |   IF formula THEN formula ELSE formula { $$ = newast_1(8, $2, $4, $6); }
  |   NOT formula { $$ = newast_1(9, $2, NULL, NULL); }
  |   primary COMPARISON primary { $$ = newast_1(10, $1, newstringval($2), $3); }
  |   call { $$ = $1; }
  ;

primary: LOWER_ID { $$ = newstringval($1); }
  | STRING_LITERAL { $$ = newstringval($1); }
  | call { $$ = $1; }
  ;
call: LOWER_ID DOT LOWER_ID LEFT_BRACKET RIGHT_BRACKET { $$ = newast_3(11, $1, NULL, $3); }
  | LOWER_ID DOT LOWER_ID LEFT_BRACKET STRING_LITERAL RIGHT_BRACKET { $$ = newast_4(11, $1, $3, $5); }
  ;
expr: UNDERSCORE { $$ = newstringval($1); }
  | primary { $$ = $1; }
  ;
%%

void yyerror(char *s, ...) {
  extern int yylineno;
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

struct ast* newast_1(int nodetype, struct ast *l, struct ast *m, struct ast *r) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));
  if (!a) {
    yyerror("out of space for constructing AST");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->m = m;
  a->r = r;
  return a;
}

struct ast* newstringval(char *value) {
  struct stringval *a = (struct stringval *)malloc(sizeof(struct stringval));
  if (!a) {
    yyerror("out of space for constructing AST");
    exit(0);
  }
  a->nodetype = 0;
  a->value = value;
  return (struct ast *)a;
}

struct ast* newast_2(int nodetype, char *valueL, char *valueM, struct ast *r) {
  struct ast *l = newstringval(valueL);
  struct ast *m = newstringval(valueM);
  return newast_1(nodetype, l, m, r);
}

struct ast* newast_3(int nodetype, char *valueL, struct ast *m, char *valueR) {
  struct ast *l = newstringval(valueL);
  struct ast *r = newstringval(valueR);
  return newast_1(nodetype, l, m, r);
}

struct ast* newast_4(int nodetype, char *valueL, char *valueM, char *valueR) {
  struct ast *l = newstringval(valueL);
  struct ast *m = newstringval(valueM);
  struct ast *r = newstringval(valueR);
  return newast_1(nodetype, l, m, r);
}

void translateImportStmt(struct ast *a) {
  // No import opts
  if (!a) {
    yyerror("no import language");
    return;
  }

  if (!a->r) {
    yyerror("error in import statement");
    return; 
  } else if (strcmp(((struct stringval *)a->r)->value, "java")) {
    yyerror("only java is supported");
    return; 
  }
}

void translateFrom(struct ast *a) {
  // No fromt opts, accept
  if (!a) {
    return;
  }

  if (a->l) {
    char *result;
    strcpy(result, ".decl ");
    char *type = ((struct stringval *)a->l)->value;
    char *name = ((struct stringval *)a->m)->value;
    strcat(result, type);
    printf("%s", result);
    findSymbol(type);
    printf("\n");
    // Store into the var declaration symbol table for future use
    storeVarSymbolTable(type, name);
  }

  if (!a->r) {
    return;
  } else {
    translateFrom(a->r);
  }
}

void translateWhere(struct ast *a) {
  // No where opts, accept
  if (!a) {
    return;
  }

  int type = a->nodetype;
  switch (type) {
    case 5: 
    case 7:
    case 8:
    case 9:
      if (a->l) translateWhere(a->l);
      if (a->m) translateWhere(a->m);
      if (a->m) translateWhere(a->r);
      break;
    case 6:
      translateWhere(a->l);
      printf(",");
      translateWhere(a->r);
      break;
    case 10:
      translateComparison(a);
      break;
    case 11:
      translateCall(a);
      break;
  }
}

void translateCall(struct ast *a) {
  // No select opts
  if (!a) {
    yyerror("error in call opts");
    return;
  }

  char *name = ((struct stringval *)a->l)->value;
  char *field = ((struct stringval *)a->m)->value;
  if (a->r) { // For direct translate call, it can be directly translate to a rule
    char *value = ((struct stringval *)a->r)->value;
    replace(name, field, value);
  } // For indirect translate call, it will be handled in comparison
  return;
}

void translateComparison(struct ast *a) {
  char *comparison = ((struct stringval *)a->m)->value;
  struct ast *l = a->l;
  struct ast *r = a->r;
  if (!l || !r) {
    yyerror("invalid comparison syntax");
    return;
  }

  if (strcmp(comparison, "=") == 0) {
    if (l->nodetype == 11 && r->nodetype == 0) {
      // Create a reference
      char *name = ((struct stringval *)l->l)->value;
      char *field = ((struct stringval *)l->r)->value;
      char referenceSymbol = replaceCreateReference(name, field);
      // Store the refernce to the var
      char *nextName = ((struct stringval *)r)->value;
      storeRuleReferenceTable(referenceSymbol, nextName);
    }
  } else {
    // I think Datalog does not provide other types of comparison like >=, <=
  }
  return;
}

void translateSelect(struct ast *a) {
  // No select opts
  if (!a) {
    yyerror("error in select opts");
    return;
  }

  char *result;
  strcpy(result, ".output ");
  if (a->l) {
    char *temp = ((struct stringval *)a->l)->value;
    strcat(result, temp);
    strcat(result, "(location: string)"); // By default, output the location
    storeOutputVar(temp);
  }
  printf("%s\n", result);

  if (!a->r) {
    return;
  } else {
    translateSelect(a->r);
    return;
  }
}

void eval(struct ast *a) {
  if(!a) {
    yyerror("internal error, null eval");
    return;
  }

  switch(a->nodetype) {
    /* import statement */
    case 1: 
      translateImportStmt(a); 
      break;
    /* select statement */
    case 2: 
      translateFrom(a->l);
      translateSelect(a->r);
      printFormulaLHS();
      translateWhere(a->m);
      break;
    default: 
      printf("internal error: bad node %c\n", a->nodetype);
  }
  return;
}

int main(int ac, char **av)
{
  // initialize the symbol table
  initialize();

  extern FILE *yyin;
  if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
    perror(av[1]);
    exit(1);
  }
  if(!yyparse())
    printf("CodeQL parse worked\n");
  else
    printf("CodeQL parse failed\n");
}
