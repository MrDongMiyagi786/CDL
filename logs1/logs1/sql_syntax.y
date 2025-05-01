%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();
void print_indent(int level);
extern FILE *yyin;
int indent = 0;
%}

%union {
    char* str;
}

%token <str> IDENTIFIER
%token SELECT FROM COMMA SEMICOLON

%%

query:
    SELECT select_list FROM IDENTIFIER SEMICOLON {
        print_indent(indent);
        printf("|-- SELECT statement\n");
        indent++;
        print_indent(indent);
        printf("|-- Columns:\n");
        indent++;
        // Columns printed in select_list rule
        indent--;
        print_indent(indent);
        printf("|-- Table: %s\n", $4);
        indent--;
    }
    ;

select_list:
    IDENTIFIER {
        print_indent(indent);
        printf("|-- Column: %s\n", $1);
    }
    | select_list COMMA IDENTIFIER {
        print_indent(indent);
        printf("|-- Column: %s\n", $3);
    }
    ;

%%

void yyerror(const char *s) {
    printf("Syntax Error: %s\n", s);
}

void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }
    
    yyin = input_file;
    yyparse();
    fclose(input_file);
    return 0;
}