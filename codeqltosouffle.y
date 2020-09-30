/* Parser for codeql subset */
%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);
void printf(char *s, ...);
%}

%union {
        char *strval;
        int intval;
        int subtok;
}

%token <strval> IMPORT
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

%type <strval> import_opts, select_opts, formula, call, primary

%start stmt_list

%%

stmt_list: stmt 
  | stmt_list stmt 
  ;

stmt: import_stmt { printf("IMPORT STMT"); }
  |   select_stmt { printf("SELECT STMT"); }
  ;

import_stmt: IMPORT import_opts { printf("IMPORT @%s", $2); } ;
import_opts: 'java' { $$ = 'java'; }
  | 'go' { $$ = 'go'; }
  | 'c#' { $$ = 'c#'; }
  | 'c'  { $$ = 'c'; }
  | 'c++' { $$ = 'c++'; }
  | 'python' { $$ = 'python'; }
  | 'javascript' { $$ = 'javscript'; }
  ;

select_stmt: SELECT select_opts { printf("SELECT @%s", $2); }
  | FROM from_opts SELECT select_opt  { printf("SELECT @%s FROM DECLARED VARIABLES", $4)}
  | FROM from_opts WHERE where_opts SELECT select_opts { printf("SELECT @%s FROM DECLARED VARIABLES UNDER CERTAIN CONDITION", $6); }
  ;

select_opts: LOWER_ID { $$ = $1 }
  | LOWER_ID ',' select_opts { $1->next = $3; $$ = $1; }
  ;

from_opts: var_decls; 
var_decls: var_decl 
  | var_decl ',' var_decls 
  ;
var_decl: UPPER_ID LOWER_ID { printf("DECLARED %s OF TYPE %s", $2, $1); } ;

where_opts: formula;
formula: fparen { $$ = 'fparen'; }
  |   disjunction { $$ = 'disjunction'; }
  |   conjunction { $$ = 'conjunction'; }
  |   implies { $$ = 'implies'; }
  |   ifthen { $$ = 'ifthen'; }
  |   negated { $$ = 'negated'; }
  |   comparison { $$ = 'comparison'; }
  |   call 
  ;
fparen: "(" formula ")" { printf("FORMULA @%s", $2); };
disjunction: formula OR formula { printf("FORMULA %s OR FORMULA %s", $1, $2); };
conjunction: formula AND formula { printf("FORMULA %s AND FORMULA %s", $1, $2); };
implies: formula IMPLIES formula { printf("FORMULA %s IMPLIES FORMULA %s", $1, $2); };
ifthen: IF formula THEN formula ELSE formula { printf("IF FORMULA %s THEN FORMULA %s ELSE FORMULA %s", $1, $2); };
negated: NOT formula { printf("NEGATED FORMULA %s", $2); };
comparison: primary COMPARISON primary { printf("COMPARE %s and %s", $1, $3)};
call: LOWER_ID "." LOWER_ID "()" { 
      string object = $1;
      string method = $2;
      $$ = object + '.' + method; }
  | LOWER_ID "." LOWER_ID "(" STRING_LITERAL ")" { 
      string object = $1;
      string method = $2;
      string param = $3;
      $$ = object + '.' + method + '(' + param + ')'; }
  ;
primary: 
  | LOWER_ID { $$ = $1 }
  | call { $$ = $1 }
  ;

%%
