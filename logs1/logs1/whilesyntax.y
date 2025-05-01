%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

void yyerror(const char *s);
int yylex();
void print_indent(int level);
int indent = 0; // Global indent level
extern FILE *yyin;  // Declare external file pointer
%}

%union {
    char* str;    // for ID and others
}
%token <str> ID NUMBER

/* Declare operator precedence */
%left PLUS MINUS
%left MUL DIV
%left LT GT EQ

%token WHILE LPAREN RPAREN LBRACE RBRACE EQ LT GT PLUS MINUS MUL DIV SEMICOLON ASSIGN

%%

program:
    while_stmt {
       //printf("\nParse Tree:\n");
       //printf("|-- program\n");
        indent++;
        /* while_stmt printing is handled by its own action */
        indent--;
    }
    ;

while_stmt:
    WHILE LPAREN condition RPAREN LBRACE stmt_list RBRACE {
        print_indent(indent);
        printf("|-- while_stmt\n");
        indent++;
            print_indent(indent);
            printf("|-- WHILE\n");
            print_indent(indent);
            printf("|-- condition\n");
            indent++;
                /* condition rule prints its structure */
            indent--;
            print_indent(indent);
            printf("|-- stmt_list\n");
            indent++;
                /* stmt_list rule prints its structure */
            indent--;
            print_indent(indent);
            printf("|-- RBRACE\n");
        indent--;
    }
    ;

condition:
    expr LT expr {
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
    }
    ;

/* Use a standard left-recursive definition (without an empty alternative)
   to avoid reduce/reduce conflicts.
   In the base case, we simulate an extra branch showing an empty additional statement.
*/
stmt_list:
    stmt {
        print_indent(indent);
        printf("|-- stmt_list\n");
        indent++;
            /* Print the stmt details (already printed in stmt rule) */
            print_indent(indent);
            printf("|-- stmt\n");
            print_indent(indent+1);
            printf("|-- (empty, if any additional statement)\n");
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

expr:
    expr PLUS expr {
        print_indent(indent);
        printf("|-- expr + expr\n");
    }
    | ID {
        print_indent(indent);
        printf("|-- ID: %s\n", $1);
    }
    | NUMBER {
        print_indent(indent);
        printf("|-- NUMBER: %s\n", $1);
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
            /* expr already prints its contents */
            print_indent(indent);
            printf("|-- SEMICOLON\n");
        indent--;
    }


%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }

    printf("Parsing while statement from file:\n");
    yyparse();
    
    fclose(yyin);
    return 0;
}