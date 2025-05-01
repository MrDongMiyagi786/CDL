%{
    // Lexical Analyzer for a subset of Java using Lex
    #include <stdio.h>
    extern FILE *yyin;
    int lno = 1;
%}

%%

"//".* {
    printf("Line %d: Lexeme '%s' Token: SINGLE_LINE_COMMENT Token-Value: %s\n", lno, yytext, yytext);
}

\/\*([^*]|\*+[^*/])*\*+\/ {
    printf("Line %d: Lexeme '%s' Token: MULTI_LINE_COMMENT Token-Value: %s\n", lno, yytext, yytext);
}

"abstract"|"assert"|"boolean"|"break"|"byte"|"case"|"catch"|"char"|"class"|"const"|"continue"|"default"|"do"|"double"|"else"|"enum"|"extends"|"final"|"finally"|"float"|"for"|"goto"|"if"|"implements"|"import"|"instanceof"|"int"|"interface"|"long"|"native"|"new"|"package"|"private"|"protected"|"public"|"return"|"short"|"static"|"strictfp"|"super"|"switch"|"synchronized"|"this"|"throw"|"throws"|"transient"|"try"|"void"|"volatile"|"while" {
    printf("Line %d: Lexeme '%s' Token: KEYWORD Token-Value: %s\n", lno, yytext, yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Line %d: Lexeme '%s' Token: IDENTIFIER Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+\.[0-9]*([eE][-+]?[0-9]+)?[fFdD]? {
    printf("Line %d: Lexeme '%s' Token: FLOAT_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+ {
    printf("Line %d: Lexeme '%s' Token: INT_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

\'([^\\\']|\\.)\' {
    printf("Line %d: Lexeme '%s' Token: CHAR_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

\"([^\\\"]|\\.)*\" {
    printf("Line %d: Lexeme '%s' Token: STRING_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

"="|"+"|"-"|"*"|"/"|"%"|"++"|"--"|"=="|"!="|"<"|"<="|">"|">="|"&&"|"||"|"!"|"&"|"|"|"^"|"~"|"<<"|">>"|">>>" {
    printf("Line %d: Lexeme '%s' Token: OPERATOR Token-Value: %s\n", lno, yytext, yytext);
}

"("|")"|"{"|"}"|"["|"]"|";"|","|"." {
    printf("Line %d: Lexeme '%s' Token: SEPARATOR Token-Value: %s\n", lno, yytext, yytext);
}

[ \t]+    { /* skip whitespace */ }

\n        { lno++; }

. {
    printf("Line %d: Lexeme '%s' Error: Unrecognized symbol\n", lno, yytext);
}

%%

int yywrap() {
    return 1;
}

int main() {
    FILE *f = fopen("java.txt", "r");
    if (!f) {
        perror("Failed to open java_input.txt");
        return 1;
    }
    yyin = f;
    printf("Reading Java code from java_input.txt...\n\n");
    yylex();
    fclose(f);
    return 0;
}
