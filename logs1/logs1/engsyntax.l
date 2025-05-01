%{
#include "engsyntax.tab.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>   

#ifndef strdup
char *strdup(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    strcpy(d, s);
    return d;
}
#endif

// Convert to lowercase for case-insensitive match
char *to_lower(const char *s) {
    char *lower = strdup(s);
    if (lower == NULL) return NULL;   // <-- Avoid NULL dereference
    for (int i = 0; lower[i]; i++) {
        lower[i] = tolower((unsigned char)lower[i]);
    }
    return lower;
}
%}

%%

[aA]|[tT][hH][eE] {
    char *word = to_lower(yytext);
    yylval.str = word;
    return ARTICLE;
}

[bB][lL][aA][cC][kK]|[wW][hH][iI][tT][eE]|[hH][aA][pP][pP][yY]|[aA][nN][gG][rR][yY] {
    yylval.str = to_lower(yytext);
    return ADJECTIVE;
}

[cC][aA][tT][sS]?|[dD][oO][gG][sS]?|[fF][iI][sS][hH]|[fF][oO][oO][dD][sS]? {
    yylval.str = to_lower(yytext);
    return NOUN;
}

[eE][aA][tT][sS]?|[lL][oO][vV][eE][sS]?|[hH][aA][tT][eE][sS]? {
    yylval.str = to_lower(yytext);
    return VERB;
}

[ \t\r\n]+   { /* skip whitespace */ }
[.?!]        { /* ignore punctuation */ }

[a-zA-Z]+    {
    char *word = to_lower(yytext);
   // printf("Unknown word: %s\n", word);
    free(word);
    return 0;
}

.            { return yytext[0]; }

.[!@#$%^&*|]  {  
    printf("unidentified token\n");
    exit(0);
}

%%

int yywrap() { return 1; }