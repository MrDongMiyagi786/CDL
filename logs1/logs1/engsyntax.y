%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();
extern FILE *yyin;  // Declare yyin for file input
%}

%union {
    char* str;
}

%token <str> ARTICLE NOUN VERB ADJECTIVE
%type <str> sentence opt_adj

%%

sentence:
    ARTICLE opt_adj NOUN VERB NOUN {
        printf("Valid sentence structure:\n");
        printf("Article: %s\n", $1);
        if ($2) printf("Adjective: %s\n", $2);
        printf("Subject: %s\n", $3);
        printf("Verb: %s\n", $4);
        printf("Object: %s\n", $5);
        
        // Free allocated memory
        free($1); free($2); free($3); free($4); free($5);
    }
    ;

opt_adj:
    /* empty */    { $$ = NULL; }
    | ADJECTIVE    { $$ = $1; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
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

    yyparse();
    
    fclose(yyin);
    return 0;
}