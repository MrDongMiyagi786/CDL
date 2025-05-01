%{
#include "sql_syntax.tab.h"
#include <string.h>
#include <stdlib.h>

#ifndef strdup
char *strdup(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    strcpy(d, s);
    return d;
}
#endif
%}

%%

"SELECT"            { return SELECT; }
"FROM"              { return FROM; }
","                 { return COMMA; }
";"                 { return SEMICOLON; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENTIFIER; }
[ \t\r\n]+          { /* skip whitespace */ }
.                   { return yytext[0]; }

%%
int yywrap() { return 1; }