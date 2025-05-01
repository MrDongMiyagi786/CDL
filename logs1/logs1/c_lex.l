%{
    // Implement lexical analyser for subset of 'C' language using LEX. 
    #include <stdio.h>
    extern FILE *yyin;
    int lno = 1;
%}

%%

"int"|"return"|"if"|"else"|"for" { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: KEYWORD  |  Token-Value: %s\n", lno, yytext, yytext); 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: IDENTIFIER  |  Token-Value: %s\n", lno, yytext, yytext); 
}

[0-9]+\.[0-9]+ { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: FLOAT  |  Token-Value: %s\n", lno, yytext, yytext); 
}

[0-9]+ { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: NUMBER  |  Token-Value: %s\n", lno, yytext, yytext); 
}

"=="|"!="|"<"|">"|"="|"+"|"-"|"*"|"/" { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: OPERATOR  |  Token-Value: %s\n", lno, yytext, yytext); 
}

"{"|"}"|"("|")"|";" { 
    printf("Line %d:  |  Lexeme '%s'  |  Token: SYMBOL  |  Token-Value: %s\n", lno, yytext, yytext); 
}

[ \t]+ { /* Skip whitespace */ }

\n { lno++; }

. { 
    printf("Line %d:  |  Lexeme '%s'  |  Error: Unrecognized symbol\n", lno, yytext); 
}

%%

int yywrap() {
    return 1;
}

int main() {
    FILE *f = fopen("input.txt", "r");
    if (!f) {
        perror("Failed to open c.txt");
        return 1;
    }

    yyin = f;
    printf("Reading C code from c.txt...\n\n");
    yylex();
    fclose(f);
    return 0;
}
