/* Parser for codeql subset */
%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);
void emit(char *s, ...);
%}

%union {
        char *strval;
        int subtok;
}

%token <strval> STRING
%token <strval> USERVAR
%token <subtok> COMPARISON /* = <> < > <= >= <=> */

%type <intval> select_opts select_expr_list

%start stmt_list

%%

stmt_list: stmt 
  | stmt_list stmt 
  ;

expr: NAME         { emit("NAME %s", $1); free($1); }
   | NAME '.' NAME { emit("FIELDNAME %s.%s", $1, $3); free($1); free($3); }
   | USERVAR       { emit("USERVAR %s", $1); free($1); }
   | STRING        { emit("STRING %s", $1); free($1); }
   ;

stmt: import_stmt { emit("STMT"); }
 ;
import_stmt: IMPORT import_opts { emit("IMPORT %d", $2); } 
 ;

stmt: select_stmt { emit("STMT"); }
 ;
select_stmt: SELECT select_opts select_expr_list { emit("SELECTNODATA %d %d", $2, $3); } ;
 | SELECT select_opts select_expr_list FROM table_references
   opt_where opt_groupby opt_having opt_orderby opt_limit
   opt_into_list { emit("SELECT %d %d %d", $2, $3, $5); } ;
;


