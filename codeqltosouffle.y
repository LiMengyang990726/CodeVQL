/* Parser for codeql subset */
%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

extern int yylex(); /* from the lexer */
void yyerror(char *s, ...);
char* concatf(char *s, ...);
%}

%union {
        char *strval;
        int intval;
        int subtok;
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
%token <strval> STRING_LITERAL
%token <strval> UPPER_ID
%token <strval> LOWER_ID
%token <subtok> COMPARISON

%type <strval> import_opts select_opts formula call primary

%start stmt_list

%%

stmt_list: stmt 
  | stmt_list stmt 
  ;

stmt: import_stmt { printf("IMPORT STMT"); }
  |   select_stmt { printf("SELECT STMT"); }
  ;

import_stmt: IMPORT import_opts { printf("IMPORT @%s", $2); } ;
import_opts: JAVA { $$ = "java"; }
  | GO { $$ = "go"; }
  | CSHARP { $$ = "c#"; }
  | 'c'  { $$ = "c"; }
  | CPP { $$ = "c++"; }
  | PYTHON { $$ = "python"; }
  | JAVASCRIPT { $$ = "javscript"; }
  ;

select_stmt: SELECT select_opts { printf("SELECT @%s", $2); }
  | FROM from_opts SELECT select_opts  { printf("SELECT @%s FROM DECLARED VARIABLES", $4); }
  | FROM from_opts WHERE where_opts SELECT select_opts { printf("SELECT @%s FROM DECLARED VARIABLES UNDER CERTAIN CONDITION", $6); }
  ;

select_opts: LOWER_ID { $$ = $1; }
  | LOWER_ID ',' select_opts { $$ = $1; }
  ;

from_opts: var_decls; 
var_decls: var_decl 
  | var_decl ',' var_decls 
  ;
var_decl: UPPER_ID LOWER_ID { printf("DECLARED %s OF TYPE %s", $2, $1); } ;

where_opts: formula;
formula: fparen { $$ = "fparen"; }
  |   disjunction { $$ = "disjunction"; }
  |   conjunction { $$ = "conjunction"; }
  |   implies { $$ = "implies"; }
  |   ifthen { $$ = "ifthen"; }
  |   negated { $$ = "negated"; }
  |   comparison { $$ = "comparison"; }
  |   call 
  ;
fparen: '(' formula ')' { printf("FORMULA @%s", $2); };
disjunction: formula OR formula { printf("FORMULA %s OR FORMULA %s", $1, $2); };
conjunction: formula AND formula { printf("FORMULA %s AND FORMULA %s", $1, $2); };
implies: formula IMPLIES formula { printf("FORMULA %s IMPLIES FORMULA %s", $1, $2); };
ifthen: IF formula THEN formula ELSE formula { printf("IF FORMULA %s THEN FORMULA %s ELSE FORMULA %s", $1, $2); };
negated: NOT formula { printf("NEGATED FORMULA %s", $2); };
comparison: primary COMPARISON primary { printf("COMPARE %s and %s", $1, $3); };
call: LOWER_ID '.' LOWER_ID "()" { $$ = concatf("%s.%s()", $1, $3); }
  | LOWER_ID '.' LOWER_ID '(' STRING_LITERAL ')' { $$ = concatf("%s.%s(%s)", $1, $3, $5); }
  ;
primary: LOWER_ID { $$ = $1; }
  | call { $$ = $1; }
  ;

%%

char* concatf(char *s, ...)
{
  va_list args;
  char* buf = NULL;
  va_start(args, s);
  int n = vasprintf(&buf, s, args);
  va_end(args);
  if (n < 0) { free(buf); buf = NULL; }
  return buf;
}

void yyerror(char *s, ...) {
  extern int yylineno;
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int main(int ac, char **av)
{
  extern FILE *yyin;
  if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
    perror(av[1]);
    exit(1);
  }
  if(!yyparse())
    printf("SQL parse worked\n");
  else
    printf("SQL parse failed\n");
} /* main */