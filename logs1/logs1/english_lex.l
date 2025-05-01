%{
    // Implement lexical analyzer for subset of English language using LEX
    #include <stdio.h>
    extern FILE *yyin;
    int lno = 1;
%}

%%

[0-9]+ { 
    printf("Line %d: Lexeme '%s' Token: NUMBER Token-Value: %s\n", lno, yytext, yytext); 
}

"I"|"you"|"he"|"she"|"it"|"we"|"they"|"me"|"him"|"her"|"us"|"them"|"my"|"your"|"his"|"its"|"our"|"their"|"mine"|"yours"|"hers"|"ours"|"theirs"|"myself"|"yourself"|"himself"|"herself"|"itself"|"ourselves"|"yourselves"|"themselves" {
    printf("Line %d: Lexeme '%s' Token: PRONOUN Token-Value: %s\n", lno, yytext, yytext);
}

[A-Z][a-z]+ {  
    printf("Line %d: Lexeme '%s' Token: NOUN Token-Value: %s\n", lno, yytext, yytext);
}

[a-z]+("able"|"ible"|"al"|"ant"|"ent"|"ary"|"ory"|"ed"|"en"|"ful"|"ic"|"ical"|"ish"|"ive"|"less"|"like"|"ly"|"ous"|"ious"|"eous"|"y"|"some"|"ing"|"ese"|"esque"|"ian"|"ar"|"ine"|"istic"|"worthy")$ {  
    printf("Line %d: Lexeme '%s' Token: ADJECTIVE Token-Value: %s\n", lno, yytext, yytext);
}

[a-z]+("ly") {  
    printf("Line %d: Lexeme '%s' Token: ADVERB Token-Value: %s\n", lno, yytext, yytext);
}

[a-z]+("ed"|"ing") {  
    printf("Line %d: Lexeme '%s' Token: VERB Token-Value: %s\n", lno, yytext, yytext);
}

[a-z]+ {  
    printf("Line %d: Lexeme '%s' Token: WORD Token-Value: %s\n", lno, yytext, yytext);
}

[ \t]+ {  
   /* skip whitespace */
}

\n { 
    lno++; 
}

['"“”] {  
    printf("Line %d: Lexeme '%s' Token: QUOTATION Token-Value: %s\n", lno, yytext, yytext);
}

[.,!?;:] {  
    printf("Line %d: Lexeme '%s' Token: PUNCTUATION Token-Value: %s\n", lno, yytext, yytext);
}

. { 
    printf("Line %d: Lexeme '%s' Error: UNRECOGNIZED SYMBOL\n", lno, yytext); 
}


%%

int yywrap() {
    return 1;
}

int main() {
    FILE *f = fopen("english.txt", "r");
    if (!f) {
        perror("Failed to open english.txt");
        return 1;
    }
    yyin = f;
    printf("Reading English text from english.txt...\n\n");
    yylex();
    fclose(f);
    return 0;
}
