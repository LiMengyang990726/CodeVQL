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
%s BTWMODE

%%

    /* keywords */
import    { return IMPORT; }
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

    /* question: (\\.) in regular expression? */

    /* strings (only allow enclosing with "") */
\"(.|\"\"|[^"\n])*\" { yylval.strval = strdup(yytext); return STRING_LITERAL; }
\"(.|\"\"|[^"\n])*$    { yyerror("Unterminated string %s", yytext); }

    /* upper id */
[A-Z][0-9a-z_.$]* { yylval.strval = strdup(yytext); return UPPER_ID; }

    /* lower id */
[0-9a-z_.$]+ { yylval.strval = strdup(yytext); return LOWER_ID; }

    /* operators */
"=" { yylval.subtok = 1; return COMPARISON; }
"!=" { yylval.subtok = 2; return COMPARISON; }
"<" { yylval.subtok = 3; return COMPARISON; }
">" { yylval.subtok = 4; return COMPARISON; }
"<=" { yylval.subtok = 5; return COMPARISON; }
">=" { yylval.subtok = 6; return COMPARISON; }

    /* comments */  
\/\*                { oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>.|\n       ;
<COMMENT>|\n\*\/    { BEGIN oldstate; }
<COMMENT><<EOF>>    { yyerror("unclosed comment"); }

    /* whitespace */
[ \t\n]         ;

    /* everything else */
.   { yyerror("mystery character '%c'", *yytext); }

%%
