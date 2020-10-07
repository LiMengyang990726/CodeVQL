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

%type <strval> import_opts select_opts as_expr expr from_opts var_decls var_decl formula call primary 

%start stmt_list

%%

stmt_list: stmt 
  | stmt_list stmt 
  ;

stmt: import_stmt
  |   select_stmt
  | where_opts
  ;

import_stmt: IMPORT import_opts { printf("IMPORT %s \n", $2); } ;
import_opts: JAVA { $$ = "java"; }
  | GO { $$ = "go"; }
  | CSHARP { $$ = "c#"; }
  | CPP  { $$ = "cpp"; }
  | PYTHON { $$ = "python"; }
  | JAVASCRIPT { $$ = "javscript"; }
  ;

select_stmt: SELECT select_opts { printf("SELECT %s \n", $2); }
  | FROM from_opts SELECT select_opts  { printf("SELECT %s FROM DECLARED VARIABLES %s \n", $4, $2); }
  | FROM from_opts WHERE where_opts SELECT select_opts { printf("SELECT %s FROM DECLARED VARIABLES %s UNDER CERTAIN CONDITION\n", $6, $2); }
  ;

select_opts: as_expr { $$ = $1; }
  | select_opts COMMA as_expr  { $$ = concatf("%s, %s", $1, $3); }
  ;
as_expr: expr { $$ = $1; }
  | expr AS LOWER_ID { $$ = $3; }
  ;
expr: UNDERSCORE { $$ = $1; }
  | primary { $$ = $1; }
  ;

from_opts: var_decls { $$ = $1; }; 
var_decls: var_decl { $$ = $1; }
  | var_decls COMMA var_decl { $$ = concatf("%s, %s", $1, $3); }
  ;
var_decl: UPPER_ID LOWER_ID { $$ = concatf("%s %s", $1, $2); } ;

where_opts: formula { printf("Condition: %s \n", $1); };
formula: fparen { $$ = "fparen"; }
  |   disjunction { $$ = "disjunction"; }
  |   conjunction { $$ = "conjunction"; }
  |   implies { $$ = "implies"; }
  |   ifthen { $$ = "ifthen"; }
  |   negated { $$ = "negated"; }
  |   comparison { $$ = "comparison"; }
  |   call { $$ = $1; }
  ;
fparen: LEFT_BRACKET formula RIGHT_BRACKET { printf("FORMULA %s\n", $2); };
disjunction: formula OR formula { printf("FORMULA %s OR FORMULA %s\n", $1, $2); };
conjunction: formula AND formula { printf("FORMULA %s AND FORMULA %s\n", $1, $2); };
implies: formula IMPLIES formula { printf("FORMULA %s IMPLIES FORMULA %s\n", $1, $2); };
ifthen: IF formula THEN formula ELSE formula { printf("IF FORMULA %s THEN FORMULA %s ELSE FORMULA %s\n", $1, $2); };
negated: NOT formula { printf("NEGATED FORMULA %s\n", $2); };
comparison: primary COMPARISON primary { printf("COMPARE %s and %s\n", $1, $3); };

primary: LOWER_ID { $$ = $1; }
  | STRING_LITERAL { $$ = $1; }
  | call { $$ = $1; }
  ;
call: LOWER_ID DOT LOWER_ID LEFT_BRACKET RIGHT_BRACKET { $$ = concatf("%s.%s()", $1, $3); }
  | LOWER_ID DOT LOWER_ID LEFT_BRACKET STRING_LITERAL RIGHT_BRACKET { $$ = concatf("%s.%s(%s)", $1, $3, $5); }
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
    printf("CodeQL parse worked\n");
  else
    printf("CodeQL parse failed\n");
}