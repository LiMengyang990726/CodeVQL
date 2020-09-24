/* Scanner for codeql subset */

%option noyywrap nodefault yylineno case-insensitive

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
IMPORT    { return IMPORT; }
FROM      { return FROM; }
WHERE     { return WHERE; }
ADD       { return ADD; }
SELECT    { return SELECT; }

   /* strings */
'(\\.|''|[^'\n])*'   |
\"(\\.|\"\"|[^"\n])*\"  { yylval.strval = strdup(yytext); return STRING; }

'(\\.|[^'\n])*$      { yyerror("Unterminated string %s", yytext); }
\"(\\.|[^"\n])*$    { yyerror("Unterminated string %s", yytext); }

   /* operators */
"="     { yylval.subtok = 4; return COMPARISON; }

   /* user variables */
@[0-9a-z_.$]+ |
@\"[^"\n]+\" |
@`[^`\n]+` |
@'[^'\n]+' { yylval.strval = strdup(yytext+1); return USERVAR; }
@\"[^"\n]*$ |
@`[^`\n]*$ |
@'[^'\n]*$ { yyerror("unterminated quoted user variable %s", yytext); }

    /* comments */   
#.*             ;
"--"[ \t].*     ;

"/*"            { oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>"*/"   { BEGIN oldstate; }
<COMMENT>.|\n   ;
<COMMENT><<EOF>> { yyerror("unclosed comment"); }

    /* everything else */
[ \t\n]         /* whitespace */
.               { yyerror("mystery character '%c'", *yytext); }

%%
