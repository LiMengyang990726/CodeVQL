/* Scanner for codeql subset */

%option noyywrap nodefault yylineno case-sensitive batch

%{
#include "codeqltosouffle.tab.h"
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);
int oldstate;
%}

%x COMMENT

%%

    /* keywords */
import    { return IMPORT; }
java      { return JAVA; }
go        { return GO; }
c#        { return CSHARP; }
cpp       { return CPP; }
python    { return PYTHON; }
javascript  { return JAVASCRIPT; }
from      { return FROM; }
where     { return WHERE; }
or        { return OR; }
and       { return AND; }
implied   { return IMPLIES; }
if        { return IF; }
else      { return ELSE; }
then      { return THEN; }
not       { return NOT; }
select    { return SELECT; }
as        { return AS; }

    /* question: (\\.) in regular expression? */

    /* strings (only allow enclosing with double quote) */
\"(\\.|\"\"|[^"\n])*\" { yylval.strval = strdup(yytext); return STRING_LITERAL; }
\"(\\.|[^"\n])*$    { yyerror("Unterminated string "); }

    /* upper id */
[A-Z][0-9a-zA-Z_$]* { yylval.strval = strdup(yytext); return UPPER_ID; }

    /* lower id */
[a-z][0-9a-zA-Z_$]+ { yylval.strval = strdup(yytext); return LOWER_ID; }

    /* operators */
"=" { yylval.subtok = 1; return COMPARISON; }
"!=" { yylval.subtok = 2; return COMPARISON; }
"<" { yylval.subtok = 3; return COMPARISON; }
">" { yylval.subtok = 4; return COMPARISON; }
"<=" { yylval.subtok = 5; return COMPARISON; }
">=" { yylval.subtok = 6; return COMPARISON; }
","  { return COMMA; }
"_"  { return UNDERSCORE; }
"("  { return LEFT_BRACKET; }
")"  { return RIGHT_BRACKET; }
"."  { return DOT; }

    /* comments */  
"/*"                { oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>"*/"       { BEGIN oldstate; }
<COMMENT>.|\n       ;
<COMMENT><<EOF>>    { yyerror("unclosed comment"); }

    /* whitespace */
[ \t\n]         ;

    /* everything else */
.   { yyerror("mystery character "); }

%%
