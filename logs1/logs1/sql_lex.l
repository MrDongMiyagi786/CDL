%{
    // Lexical Analyzer for a subset of SQL using Lex
    #include <stdio.h>
    extern FILE *yyin;
    int lno = 1;
%}

%%

"--"[^\n]* {
    printf("Line %d: Lexeme '%s' Token: SINGLE_LINE_COMMENT\n", lno, yytext);
}

\/\*([^*]|\*+[^*/])*\*+\/ {
    printf("Line %d: Lexeme '%s' Token: MULTI_LINE_COMMENT\n", lno, yytext);
}

"SELECT"|"FROM"|"WHERE"|"INSERT"|"INTO"|"VALUES"|"UPDATE"|"SET"|"DELETE"|"CREATE"|"TABLE"|"DROP"|"ALTER"|"ADD"|"JOIN"|"ON"|"AS"|"GROUP"|"BY"|"ORDER"|"HAVING"|"DISTINCT"|"AND"|"OR"|"NOT"|"IN"|"IS"|"NULL"|"LIKE"|"BETWEEN"|"EXISTS"|"UNION"|"ALL"|"CASE"|"WHEN"|"THEN"|"ELSE"|"END"|"LIMIT"|"OFFSET" {
    printf("Line %d: Lexeme '%s' Token: KEYWORD Token-Value: %s\n", lno, yytext, yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("Line %d: Lexeme '%s' Token: IDENTIFIER Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+\.[0-9]+ {
    printf("Line %d: Lexeme '%s' Token: FLOAT_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

[0-9]+ {
    printf("Line %d: Lexeme '%s' Token: INT_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

\'([^\\\']|\\.)*\' {
    printf("Line %d: Lexeme '%s' Token: STRING_LITERAL Token-Value: %s\n", lno, yytext, yytext);
}

"="|"!="|"<>"|"<="|">="|"<"|">"|"+"|"-"|"*"|"/"|"%" {
    printf("Line %d: Lexeme '%s' Token: OPERATOR Token-Value: %s\n", lno, yytext, yytext);
}

"("|")"|","|";"|"\." {
    printf("Line %d: Lexeme '%s' Token: SYMBOL Token-Value: %s\n", lno, yytext, yytext);
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
    FILE *f = fopen("sql.txt", "r");
    if (!f) {
        perror("Failed to open input.txt");
        return 1;
    }
    yyin = f;
    printf("Reading SQL code from input.txt...\n\n");
    yylex();
    fclose(f);
    return 0;
}
