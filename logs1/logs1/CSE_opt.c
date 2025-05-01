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

void commonSubexpressionElimination() {
    printf("\nPerforming Common Subexpression Elimination...\n");
    bool changed = false;
    
    for (int i = 0; i < quad_count; i++) {
        if (quad_list[i].eliminated || strcmp(quad_list[i].op, "=") == 0) 
            continue;

        for (int j = 0; j < i; j++) {
            if (quad_list[j].eliminated || strcmp(quad_list[j].op, "=") == 0)
                continue;

            if (strcmp(quad_list[i].op, quad_list[j].op) == 0 &&
                strcmp(quad_list[i].arg1, quad_list[j].arg1) == 0 &&
                strcmp(quad_list[i].arg2, quad_list[j].arg2) == 0) 
            {
                printf("  Eliminating Quad %d (%s %s %s %s) - duplicates Quad %d\n",
                      i+1, quad_list[i].op, quad_list[i].arg1, 
                      quad_list[i].arg2, quad_list[i].result, j+1);
                
                // Replace all uses of result with the earlier version
                for (int k = i+1; k < quad_count; k++) {
                    if (strcmp(quad_list[k].arg1, quad_list[i].result) == 0) {
                        strcpy(quad_list[k].arg1, quad_list[j].result);
                    }
                    if (strcmp(quad_list[k].arg2, quad_list[i].result) == 0) {
                        strcpy(quad_list[k].arg2, quad_list[j].result);
                    }
                }
                
                quad_list[i].eliminated = true;
                changed = true;
                break;
            }
        }
    }
    
    if (!changed) {
        printf("  No common subexpressions found for elimination.\n");
    }
    printf("------------------------------------------------\n");
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
    commonSubexpressionElimination();
    printQuads("After Common Subexpression Elimination");

    return 0;
}