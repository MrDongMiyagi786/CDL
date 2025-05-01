%{
    // Lexical Analyzer for a subset of Python using Lex
    #include <stdio.h>
    extern FILE *yyin;
    int lno = 1;
%}

%%

"def"|"return"|"if"|"elif"|"else"|"for"|"while"|"in"|"and"|"or"|"not"|"import"|"from"|"as"|"class"|"pass"|"break"|"continue"|"True"|"False"|"None" {
    printf("Line %d: Lexeme '%s' Token: KEYWORD Token-Value: %s\n", lno, yytext, yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Line %d: Lexeme '%s' Token: IDENTIFIER Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+\.[0-9]+ {
    printf("Line %d: Lexeme '%s' Token: FLOAT Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+ {
    printf("Line %d: Lexeme '%s' Token: INTEGER Token-Value: %s\n", lno, yytext, yytext);
}

\"([^\\\"]|\\.)*\" {
    printf("Line %d: Lexeme '%s' Token: STRING Token-Value: %s\n", lno, yytext, yytext);
}

\'([^\\\']|\\.)*\' {
    printf("Line %d: Lexeme '%s' Token: STRING Token-Value: %s\n", lno, yytext, yytext);
}

"=="|"!="|"<="|">="|"<"|">"|"="|"+"|"-"|"*"|"/"|"%"|"//"|"**" {
    printf("Line %d: Lexeme '%s' Token: OPERATOR Token-Value: %s\n", lno, yytext, yytext);
}

"("|")"|"["|"]"|"{"|"}"|":"|","|"."|";"|"@"|"->"|"//="|"%="|"**="|"+="|"-="|"*="|"/="|"=" {
    printf("Line %d: Lexeme '%s' Token: SYMBOL Token-Value: %s\n", lno, yytext, yytext);
}

[ \t]+   { /* skip spaces and tabs */ }

\n        { lno++; }

"#".*     { /* skip comment */ }

. {
    printf("Line %d: Lexeme '%s' Error: Unrecognized symbol\n", lno, yytext);
}

%%

int yywrap() {
    return 1;
}

int main() {
    FILE *f = fopen("py.txt", "r");
    if (!f) {
        perror("Failed to open input.txt");
        return 1;
    }
    yyin = f;
    printf("Reading Python code from input.txt...\n\n");
    yylex();
    fclose(f);
    return 0;
}
