%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 1000
#define MAX_LINES 100

typedef struct {
    char word[100];
    int count;
    int lines[MAX_LINES];
    int lineCount;
} WordEntry;

WordEntry words[MAX_WORDS];
int word_count = 0;
int line_no = 1;

void toLower(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = tolower(s[i]);
    }
}

void addWord(char *text) {
    toLower(text);
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i].word, text) == 0) {
            words[i].count++;
            if (words[i].lines[words[i].lineCount - 1] != line_no) {
                words[i].lines[words[i].lineCount++] = line_no;
            }
            return;
        }
    }
    strcpy(words[word_count].word, text);
    words[word_count].count = 1;
    words[word_count].lines[0] = line_no;
    words[word_count].lineCount = 1;
    word_count++;
}
%}

%%
[a-zA-Z]+       { addWord(yytext); }
\n              { line_no++; }
[ \t\r]+        { /* ignore whitespace */ }
.               { /* ignore punctuation */ }
%%

int yywrap() {
    return 1;
}

int main(int argc, char **argv) {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        printf("Could not open the file!\n");
        return 1;
    }

    yyin = file;
    yylex();
    fclose(file);

    printf("\nWord\t\tCount\tLine Numbers\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < word_count; i++) {
        printf("%-15s\t%-5d\t", words[i].word, words[i].count);
        for (int j = 0; j < words[i].lineCount; j++) {
            printf("%d ", words[i].lines[j]);
        }
        printf("\n");
    }

    return 0;
}


