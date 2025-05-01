#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_STRING,
    TOKEN_SYMBOL,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

// Keywords
const char* keywords[] = {
    "if", "else", "while", "for", "int", "float", "char", "return", "void", "main"
};

// Maximum token length
#define MAX_TOKEN_LENGTH 100
#define MAX_FILE_SIZE 1024

// Current position in input
char* current;
int line = 1;
int column = 1;

// Function to check if a string is a keyword
int isKeyword(const char* str) {
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to create a new token
Token* createToken(TokenType type, const char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        fprintf(stderr, "Memory allocation failed for token\n");
        exit(EXIT_FAILURE);
    }
    
    token->type = type;
    token->value = strdup(value);
    if (token->value == NULL) {
        fprintf(stderr, "Memory allocation failed for token value\n");
        free(token);
        exit(EXIT_FAILURE);
    }
    
    token->line = line;
    token->column = column - strlen(value);
    return token;
}

Token* getNextToken() {
    // Skip whitespace
    while (isspace(*current)) {
        if (*current == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        current++;
    }

    // Check for end of input
    if (*current == '\0') {
        return createToken(TOKEN_EOF, "EOF");
    }

    // Handle identifiers and keywords
    if (isalpha(*current) || *current == '_') {
        char buffer[MAX_TOKEN_LENGTH];
        int i = 0;
        while ((isalnum(*current) || *current == '_') && i < MAX_TOKEN_LENGTH - 1) {
            buffer[i++] = *current++;
            column++;
        }
        buffer[i] = '\0';

        if (isKeyword(buffer)) {
            return createToken(TOKEN_KEYWORD, buffer);
        } else {
            return createToken(TOKEN_IDENTIFIER, buffer);
        }
    }

    // Handle numbers
    if (isdigit(*current)) {
        char buffer[MAX_TOKEN_LENGTH];
        int i = 0;
        while ((isdigit(*current) || *current == '.') && i < MAX_TOKEN_LENGTH - 1) {
            buffer[i++] = *current++;
            column++;
        }
        buffer[i] = '\0';
        return createToken(TOKEN_NUMBER, buffer);
    }

    // Handle strings
    if (*current == '"') {
        char buffer[MAX_TOKEN_LENGTH];
        int i = 0;
        current++;
        column++;
        while (*current != '"' && *current != '\0' && i < MAX_TOKEN_LENGTH - 1) {
            buffer[i++] = *current++;
            column++;
        }
        if (*current == '"') {
            current++;
            column++;
        } else {
            return createToken(TOKEN_ERROR, "Unterminated string");
        }
        buffer[i] = '\0';
        return createToken(TOKEN_STRING, buffer);
    }

    // Handle operators and symbols
    if (strchr("+-*/=<>!&|", *current)) {
        char buffer[3] = {0};
        buffer[0] = *current++;
        column++;
        
        // Check for two-character operators
        if (strchr("=<>|&", *current)) {
            buffer[1] = *current++;
            column++;
        }
        
        return createToken(TOKEN_OPERATOR, buffer);
    }

    // Handle single-character symbols
    if (strchr("(){}[];,:", *current)) {
        char buffer[2] = {*current++, '\0'};
        column++;
        return createToken(TOKEN_SYMBOL, buffer);
    }

    // Unknown character
    char buffer[2] = {*current++, '\0'};
    column++;
    return createToken(TOKEN_ERROR, buffer);
}

// Function to print token information
void printToken(Token* token) {
    const char* typeNames[] = {
        "IDENTIFIER", "NUMBER", "OPERATOR", "KEYWORD",
        "STRING", "SYMBOL", "EOF", "ERROR"
    };
    
    printf("Token: %-12s Value: %-10s Line: %d, Column: %d\n",
           typeNames[token->type], token->value, token->line, token->column);
}

// Function to free token memory
void freeToken(Token* token) {
    free(token->value);
    free(token);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Read the entire file
    char* input = (char*)malloc(MAX_FILE_SIZE);
    if (!input) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    size_t fileSize = fread(input, 1, MAX_FILE_SIZE - 1, file);
    input[fileSize] = '\0';
    fclose(file);

    current = input;
    
    printf("Lexical Analysis Results:\n");
    printf("------------------------\n");
    
    Token* token;
    while (1) {
        token = getNextToken();
        printToken(token);
        if (token->type == TOKEN_EOF || token->type == TOKEN_ERROR) {
            freeToken(token);
            break;
        }
        freeToken(token);
    }
    
    free(input);
    return 0;
}