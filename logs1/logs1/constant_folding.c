#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_QUADS 500
#define MAX_STR 100

typedef struct {
    char op[20];
    char arg1[MAX_STR];
    char arg2[MAX_STR];
    char result[MAX_STR];
    bool eliminated;
} Quadruple;

Quadruple quad_list[MAX_QUADS];
int quad_count = 0;

bool isConstant(const char *s) {
    if(s == NULL || *s == '\0' || strcmp(s, "") == 0) return false;
    int i = 0;
    if(s[0]=='-' || s[0]=='+') { if (s[1] == '\0') return false; i++; }
    for(; s[i] != '\0'; i++) if(!isdigit(s[i])) return false;
    return i > 0 && s[i-1] != '-' && s[i-1] != '+'; 
}

int evaluateExpression(const char *op, int c1, int c2) {
    if(strcmp(op, "+") == 0) return c1 + c2;
    if(strcmp(op, "-") == 0) return c1 - c2;
    if(strcmp(op, "*") == 0) return c1 * c2;
    if(strcmp(op, "/") == 0) { 
        if (c2 == 0) {
            fprintf(stderr, "Warning: Division by zero\n");
            return 0;
        }
        return c1 / c2;
    }
    return 0;
}

bool readQuadsFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening Quad input file");
        return false;
    }
    
    printf("Reading Quads from %s...\n", filename);
    quad_count = 0;
    char line[100];

    while (fgets(line, sizeof(line), fp) && quad_count < MAX_QUADS) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* parts[4] = {NULL, NULL, NULL, NULL};
        char* current_pos = line;
        int part_index = 0;

        while(part_index < 4 && current_pos != NULL && *current_pos != '\0') {
            parts[part_index++] = current_pos;
            char* next_space = strchr(current_pos, ' ');
            if (next_space != NULL) {
                *next_space = '\0';
                current_pos = next_space + 1;
            } else {
                current_pos = NULL;
            }
        }

        if (parts[0]) {
            strncpy(quad_list[quad_count].op, parts[0], 19);
            quad_list[quad_count].op[19] = '\0';

            strncpy(quad_list[quad_count].arg1, (parts[1] && strcmp(parts[1], "-") != 0) ? parts[1] : "", MAX_STR-1);
            strncpy(quad_list[quad_count].arg2, (parts[2] && strcmp(parts[2], "-") != 0) ? parts[2] : "", MAX_STR-1);
            strncpy(quad_list[quad_count].result, (parts[3] && strcmp(parts[3], "-") != 0) ? parts[3] : "", MAX_STR-1);

            quad_list[quad_count].eliminated = false;
            quad_count++;
        }
    }

    fclose(fp);
    printf("Read %d quads.\n", quad_count);
    return true;
}

void printQuads(const char* title) {
    printf("\n--- %s ---\n", title);
    printf("Nr | %-15s | %-25s | %-25s | %-25s\n", "Op", "Arg1", "Arg2", "Result");
    printf("---|-----------------|---------------------------|---------------------------|---------------------------\n");
    
    for (int i = 0; i < quad_count; i++) {
        if (!quad_list[i].eliminated) {
            printf("%03d| %-15s | %-25s | %-25s | %-25s\n", 
                   i+1, quad_list[i].op, quad_list[i].arg1, quad_list[i].arg2, quad_list[i].result);
        }
    }
    printf("--------------------------------------------------------------------------------\n");
}

void constantFoldingOptimization() {
    printf("\nPerforming Constant Folding...\n");
    bool changed = false;
    char buffer[MAX_STR];
    
    for (int i = 0; i < quad_count; i++) {
        if (quad_list[i].eliminated) continue;

        // Only process arithmetic operations
        if (strcmp(quad_list[i].op, "+") == 0 || strcmp(quad_list[i].op, "-") == 0 ||
            strcmp(quad_list[i].op, "*") == 0 || strcmp(quad_list[i].op, "/") == 0) {
            
            // Check if both arguments are constants
            if (isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2)) {
                int c1 = atoi(quad_list[i].arg1);
                int c2 = atoi(quad_list[i].arg2);
                int result = evaluateExpression(quad_list[i].op, c1, c2);
                
                sprintf(buffer, "%d", result);
                printf("  Folding Quad %d: %s %s %s %s -> %s = %s\n", 
                       i+1, quad_list[i].op, quad_list[i].arg1, 
                       quad_list[i].arg2, quad_list[i].result,
                       quad_list[i].result, buffer);
                
                // Convert to simple assignment
                strcpy(quad_list[i].op, "=");
                strcpy(quad_list[i].arg1, buffer);
                quad_list[i].arg2[0] = '\0';
                changed = true;
            }
        }
    }
    
    if (!changed) {
        printf("  No constant folding opportunities found.\n");
    }
    printf("----------------------------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <quad_file>\n", argv[0]);
        return 1;
    }

    if (!readQuadsFromFile(argv[1])) {
        fprintf(stderr, "Failed to read input file\n");
        return 1;
    }

    printQuads("Initial Quads");
    constantFoldingOptimization();
    printQuads("After Constant Folding");

    return 0;
}