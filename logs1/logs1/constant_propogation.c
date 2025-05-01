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

        // Initialize all fields
        quad_list[quad_count].op[0] = '\0';
        quad_list[quad_count].arg1[0] = '\0';
        quad_list[quad_count].arg2[0] = '\0';
        quad_list[quad_count].result[0] = '\0';

        char op[20], arg1[MAX_STR], arg2[MAX_STR], result[MAX_STR];
        int num_items = sscanf(line, "%19s %99s %99s %99s", op, arg1, arg2, result);

        strcpy(quad_list[quad_count].op, op);

        // Handle assignment operations (= value var)
        if (strcmp(op, "=") == 0) {
            if (num_items == 3) {
                strcpy(quad_list[quad_count].arg1, arg1);  // The constant value
                strcpy(quad_list[quad_count].result, arg2); // The variable
            } else {
                fprintf(stderr, "Invalid assignment format: %s\n", line);
                continue;
            }
        }
        // Handle binary operations (op arg1 arg2 result)
        else if (num_items == 4) {
            strcpy(quad_list[quad_count].arg1, arg1);
            strcpy(quad_list[quad_count].arg2, arg2);
            strcpy(quad_list[quad_count].result, result);
        }
        else {
            fprintf(stderr, "Invalid quad format: %s\n", line);
            continue;
        }

        quad_list[quad_count].eliminated = false;
        quad_count++;
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
                  i+1, quad_list[i].op,
                  quad_list[i].arg1[0] ? quad_list[i].arg1 : "-",
                  quad_list[i].arg2[0] ? quad_list[i].arg2 : "-",
                  quad_list[i].result[0] ? quad_list[i].result : "-");
        }
    }
    printf("--------------------------------------------------------------------------------\n");
}

void constantPropagationOptimization() {
    printf("\nPerforming Constant Propagation (with Folding)...\n");
    bool changed = true;
    int pass = 1;
    
    while (changed && pass <= 5) {
        changed = false;
        printf("\nPass %d:\n", pass++);
        
        // First do constant propagation
        for (int i = 0; i < quad_count; i++) {
            if (quad_list[i].eliminated) continue;
            
            // Look for constant assignments (= value var)
            if (strcmp(quad_list[i].op, "=") == 0 && isConstant(quad_list[i].arg1)) {
                const char* const_val = quad_list[i].arg1;
                const char* const_var = quad_list[i].result;
                
                // Propagate to subsequent quads
                for (int j = i + 1; j < quad_count; j++) {
                    if (quad_list[j].eliminated) continue;
                    
                    bool propagated = false;
                    
                    // Check arg1
                    if (strcmp(quad_list[j].arg1, const_var) == 0) {
                        strcpy(quad_list[j].arg1, const_val);
                        propagated = true;
                    }
                    
                    // Check arg2
                    if (strcmp(quad_list[j].arg2, const_var) == 0) {
                        strcpy(quad_list[j].arg2, const_val);
                        propagated = true;
                    }
                    
                    if (propagated) {
                        printf("  Propagated %s=%s to Quad %d\n", const_var, const_val, j+1);
                        changed = true;
                    }
                }
            }
        }
        
        // Then do constant folding
        char buffer[MAX_STR];
        for (int i = 0; i < quad_count; i++) {
            if (quad_list[i].eliminated) continue;
            
            if ((strcmp(quad_list[i].op, "+") == 0 || strcmp(quad_list[i].op, "-") == 0 ||
                 strcmp(quad_list[i].op, "*") == 0 || strcmp(quad_list[i].op, "/") == 0) &&
                isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2)) {
                
                int c1 = atoi(quad_list[i].arg1);
                int c2 = atoi(quad_list[i].arg2);
                int result = evaluateExpression(quad_list[i].op, c1, c2);
                
                sprintf(buffer, "%d", result);
                printf("  Folded Quad %d: %s %s %s -> %s = %s\n", 
                      i+1, quad_list[i].op, quad_list[i].arg1, 
                      quad_list[i].arg2, quad_list[i].result, buffer);
                
                // Convert to assignment
                strcpy(quad_list[i].op, "=");
                strcpy(quad_list[i].arg1, buffer);
                quad_list[i].arg2[0] = '\0';
                changed = true;
            }
        }
    }
    
    printf("\nConstant Propagation complete.\n");
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
    constantPropagationOptimization();
    printQuads("After Constant Propagation");

    return 0;
}