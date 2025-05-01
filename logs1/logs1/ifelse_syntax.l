%{
#include "ifelse_syntax.tab.h"
#include <string.h>
#include <stdlib.h>

#ifndef strdup
char *strdup(const char *s) {
    char *d = malloc(strlen(s) + 1);   // allocate memory
    if (d == NULL) return NULL;        // check for malloc failure
    strcpy(d, s);                      // copy string
    return d;
}
#endif
%}

%%

"if"         { return IF; }
"else"       { return ELSE; }
"("          { return LPAREN; }
")"          { return RPAREN; }
"{"          { return LBRACE; }
"}"          { return RBRACE; }
[0-9]+       { yylval.str = strdup(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return ID; }
"="          { return ASSIGN; }
"=="         { return EQ; }
"<"          { return LT; }
">"          { return GT; }
"+"          { return PLUS; }
"-"          { return MINUS; }
"*"          { return MUL; }
"/"          { return DIV; }
";"          { return SEMICOLON; }
[ \t\n]+     { /* ignore whitespace */ }
.            { return yytext[0]; }

%%

int yywrap() { return 1; }
