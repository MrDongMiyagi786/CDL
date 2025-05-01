%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

void yyerror(const char *s);
int yylex();
void print_indent(int level);
extern FILE *yyin;
int indent = 0; // Global indent level
%}

%union {
    char* str;    // for ID and others
}
%token <str> ID NUMBER

/* Declare operator precedence */
%left PLUS MINUS
%left MUL DIV
%left LT GT EQ

%token IF ELSE LPAREN RPAREN LBRACE RBRACE EQ LT GT PLUS MINUS MUL DIV SEMICOLON ASSIGN

%%

program:
    if_else_stmt {
        indent++;
        /* if_else_stmt printing is handled by its own action */
        indent--;
    }
    ;

if_else_stmt:
    IF LPAREN condition RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE {
        print_indent(indent);
        printf("|-- if_else_stmt\n");
        indent++;
            print_indent(indent);
            printf("|-- IF\n");
            print_indent(indent);
            printf("|-- condition\n");
            indent++;
                /* condition rule prints its structure */
            indent--;
            print_indent(indent);
            printf("|-- stmt_list (if-block)\n");
            indent++;
                /* if stmt_list rule prints its structure */
            indent--;
            print_indent(indent);
            printf("|-- ELSE\n");
            print_indent(indent);
            printf("|-- stmt_list (else-block)\n");
            indent++;
                /* else stmt_list rule prints its structure */
            indent--;
        indent--;
    }
    ;

condition:
    expr LT expr {
        print_indent(indent);
        printf("|-- condition\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- LT\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | expr GT expr {
        print_indent(indent);
        printf("|-- condition\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- GT\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | expr EQ expr {
        print_indent(indent);
        printf("|-- condition\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- EQ\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    ;

stmt_list:
    stmt {
        print_indent(indent);
        printf("|-- stmt_list\n");
        indent++;
            print_indent(indent);
            printf("|-- stmt\n");
        indent--;
    }
    | stmt_list stmt {
        print_indent(indent);
        printf("|-- stmt_list\n");
        indent++;
            print_indent(indent);
            printf("|-- stmt\n");
        indent--;
    }
    ;

stmt:
    ID ASSIGN expr SEMICOLON {
        print_indent(indent);
        printf("|-- stmt\n");
        indent++;
            print_indent(indent);
            printf("|-- ID: %s\n", $1);
            print_indent(indent);
            printf("|-- ASSIGN\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* expr already prints its contents */
            indent--;
            print_indent(indent);
            printf("|-- SEMICOLON\n");
        indent--;
    }
    ;

expr:
    expr PLUS expr {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- PLUS\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | expr MINUS expr {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- MINUS\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | expr MUL expr {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- MUL\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | expr DIV expr {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Left expr printed by expr rule */
            indent--;
            print_indent(indent);
            printf("|-- DIV\n");
            print_indent(indent);
            printf("|-- expr\n");
            indent++;
                /* Right expr printed by expr rule */
            indent--;
        indent--;
    }
    | ID {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- ID: %s\n", $1);
        indent--;
    }
    | NUMBER {
        print_indent(indent);
        printf("|-- expr\n");
        indent++;
            print_indent(indent);
            printf("|-- NUMBER: %s\n", $1);
        indent--;
    }
    ;

%%

void yyerror(const char *s) {
    printf("Error: %s\n", s);
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