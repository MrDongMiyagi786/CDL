//optimization in c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_QUADS 500
#define MAX_STR 100
#define MAX_BLOCKS 100

typedef struct {
    char op[20];
    char arg1[MAX_STR];
    char arg2[MAX_STR];
    char result[MAX_STR];
    bool eliminated; 
    bool is_leader; 
} Quadruple;


typedef struct {
    int start_quad; 
    int end_quad;   
    int successors[2]; 
    int num_successors;
} BasicBlock;


Quadruple quad_list[MAX_QUADS];
int quad_count = 0;
BasicBlock blocks[MAX_BLOCKS];
int block_count = 0;


bool readQuadsFromFile(const char* filename);
void printQuads(const char* title, bool show_eliminated);
void identifyBasicBlocks();
void printBasicBlocks();
void printControlFlowGraph();


void commonSubexpressionElimination();
void constantFoldingOptimization(); 
void constantPropagationOptimization(); 

bool isConstant(const char *s);
int evaluateExpression(const char *op, int c1, int c2);


bool readQuadsFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening Quad input file");
        return false;
    }
    printf("Reading Quads from %s...\n", filename);
    quad_count = 0;
    char line[100]; // Use MAX_LINE defined earlier

    while (fgets(line, sizeof(line), fp) && quad_count < MAX_QUADS) {
        line[strcspn(line, "\r\n")] = 0; // Remove trailing newline
        if (strlen(line) == 0) continue; // Skip empty lines

        char* parts[4] = {NULL, NULL, NULL, NULL}; // Pointers to parts within the line buffer
        char* current_pos = line;
        int part_index = 0;

        
        while(part_index < 4 && current_pos != NULL && *current_pos != '\0') {
             parts[part_index++] = current_pos;
             char* next_space = strchr(current_pos, ' ');
             if (next_space != NULL) {
                 *next_space = '\0';
                 current_pos = next_space + 1; // Move to the start of the next part
             } else {
                 // This is the last part
                 current_pos = NULL; // Stop the loop
             }
        }

     
        if (parts[0]) { // Op must exist
            strncpy(quad_list[quad_count].op, parts[0], 19);
            quad_list[quad_count].op[19] = '\0';

            strncpy(quad_list[quad_count].arg1, (parts[1] && strcmp(parts[1], "-") != 0) ? parts[1] : "", MAX_STR - 1);
            quad_list[quad_count].arg1[MAX_STR - 1] = '\0';

            strncpy(quad_list[quad_count].arg2, (parts[2] && strcmp(parts[2], "-") != 0) ? parts[2] : "", MAX_STR - 1);
            quad_list[quad_count].arg2[MAX_STR - 1] = '\0';

            strncpy(quad_list[quad_count].result, (parts[3] && strcmp(parts[3], "-") != 0) ? parts[3] : "", MAX_STR - 1);
            quad_list[quad_count].result[MAX_STR - 1] = '\0';

            quad_list[quad_count].eliminated = false;
            quad_list[quad_count].is_leader = false;
            quad_count++;
        } else {
             fprintf(stderr, "Warning: Skipping malformed quad line (empty?): %s\n", line);
        }
    }

    fclose(fp);
    printf("Read %d quads.\n", quad_count);
    return true;
}

void printQuads(const char* title, bool show_eliminated) {
    printf("\n--- %s ---\n", title);
    printf("Nr | %-15s | %-25s | %-25s | %-25s | %s\n", "Op", "Arg1", "Arg2", "Result", show_eliminated ? "Status" : "");
    printf("---|-----------------|---------------------------|---------------------------|---------------------------|--------\n");
    if (quad_count == 0) {
        printf("(No Quads)\n");
    }
    int displayed_count = 0;
    for (int i = 0; i < quad_count; i++) {
        if (!quad_list[i].eliminated || show_eliminated) {
            displayed_count++;
             printf("%03d| %-15s | %-25s | %-25s | %-25s | %s\n", i + 1,
                   quad_list[i].op, quad_list[i].arg1, quad_list[i].arg2, quad_list[i].result,
                   show_eliminated ? (quad_list[i].eliminated ? "(Elim)" : "") : "");
        }
    }
    if (displayed_count == 0 && !show_eliminated) {
         printf("(No quads remaining after optimization)\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");
}


void identifyBasicBlocks() {
    if (quad_count == 0) return;

 
    block_count = 0;

 
    quad_list[0].is_leader = true;


    int current_block_start = -1;
    for (int i = 0; i < quad_count; i++) {
        if (quad_list[i].is_leader) {
            // End the previous block if one was started
            if (current_block_start != -1 && block_count > 0) {
                blocks[block_count - 1].end_quad = i - 1;
            }
            // Start a new block
            if (block_count < MAX_BLOCKS) {
                blocks[block_count].start_quad = i;
                blocks[block_count].num_successors = 0; // Initialize successors
                current_block_start = i;
                block_count++;
            } else {
                fprintf(stderr, "Error: Maximum number of basic blocks exceeded.\n");
                return;
            }
        }
    }

    if (block_count > 0) {
        blocks[block_count - 1].end_quad = quad_count - 1;
    }
     printf("Found %d basic block(s).\n", block_count);
}

void printBasicBlocks() {
    printf("\n--- Basic Blocks ---\n");
    for(int i=0; i<block_count; ++i) {
        printf("Block B%d: Quads %d to %d\n", i, blocks[i].start_quad + 1, blocks[i].end_quad + 1);
    
        for (int q = blocks[i].start_quad; q <= blocks[i].end_quad; ++q) {
             printf("  %03d| %s %s %s %s\n", q + 1,
                   quad_list[q].op, quad_list[q].arg1, quad_list[q].arg2, quad_list[q].result);
        }
    }
     printf("---------------------\n");
}

void printControlFlowGraph() {
     printf("\n--- Control Flow Graph (Simplified) ---\n");

     if (block_count == 0) {
         printf("(No blocks)\n");
     }
     for(int i=0; i<block_count; ++i) {
        printf("Block B%d:", i);

        if (i + 1 < block_count) { 
             printf(" -> B%d (sequential)", i + 1);
     
        } else {
             printf(" -> END"); // Last block exits
        }
        printf("\n");
     }
      printf("---------------------------------------\n");
}


void commonSubexpressionElimination() {
    printf("\nPerforming Common Subexpression Elimination...\n");
    bool eliminated_found = false;
    for (int i = 0; i < quad_count; i++) {
        if (quad_list[i].eliminated) continue;

        // Skip assignment operations
        if (strcmp(quad_list[i].op, "=") == 0) continue;

        for (int j = 0; j < i; j++) {
            if (quad_list[j].eliminated) continue;
            if (strcmp(quad_list[j].op, "=") == 0) continue;

            if (strcmp(quad_list[i].op, quad_list[j].op) == 0 &&
                strcmp(quad_list[i].arg1, quad_list[j].arg1) == 0 &&
                strcmp(quad_list[i].arg2, quad_list[j].arg2) == 0)
            {
                printf("  Found potential CSE: Quad %d (%s %s %s %s) same as Quad %d (%s %s %s %s)\n",
                       i + 1, quad_list[i].op, quad_list[i].arg1, quad_list[i].arg2, quad_list[i].result,
                       j + 1, quad_list[j].op, quad_list[j].arg1, quad_list[j].arg2, quad_list[j].result);
                printf("    Eliminating Quad %d. Uses of '%s' should be replaced by '%s'\n",
                       i + 1, quad_list[i].result, quad_list[j].result);

                quad_list[i].eliminated = true;
                eliminated_found = true;
                break;
            }
        }
    }
    if (!eliminated_found) printf("  No common subexpressions identified for elimination.\n");
    printf("----------------------------------------\n");
}



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
    if(strcmp(op, "/") == 0) { if (c2 == 0) return 0; return c1 / c2; } // Basic div zero check
    return 0;
}


void constantFoldingOptimization() {
    printf("\nPerforming Constant Folding...\n");

    bool changed = false;
    char buffer[MAX_STR];
    char orig_arg1[MAX_STR], orig_arg2[MAX_STR], orig_op[20];

    for (int i = 0; i < quad_count; i++) {
        strcpy(orig_arg1, quad_list[i].arg1); strcpy(orig_arg2, quad_list[i].arg2); strcpy(orig_op, quad_list[i].op);

        if ((strcmp(quad_list[i].op, "+") == 0 || strcmp(quad_list[i].op, "-") == 0 ||
             strcmp(quad_list[i].op, "*") == 0 || strcmp(quad_list[i].op, "/") == 0) &&
            isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2))
        {
            int c1 = atoi(quad_list[i].arg1); int c2 = atoi(quad_list[i].arg2);
            if (strcmp(quad_list[i].op, "/") == 0 && c2 == 0) { /* handle error */ continue; }
            int result_val = evaluateExpression(quad_list[i].op, c1, c2);
            sprintf(buffer, "%d", result_val);
            printf("  Folding Quad %d (%s = %s %s %s) -> (%s = %s)\n", i + 1,
                   quad_list[i].result, orig_arg1, orig_op, orig_arg2, quad_list[i].result, buffer);
            strcpy(quad_list[i].op, "="); strcpy(quad_list[i].arg1, buffer); quad_list[i].arg2[0] = '\0';
            changed = true;
        }
    }
    if (!changed) printf("  No constant folding opportunities found (expected for this IR).\n");
    printf("----------------------------------------\n");
}

void constantPropagationOptimization() {
     printf("\nPerforming Constant Propagation (and Folding)...\n");
  
     bool changed_prop = false;
     bool changed_fold = false;

     for (int i = 0; i < quad_count; i++) {
      
         if (strcmp(quad_list[i].op, "=") == 0 && isConstant(quad_list[i].arg1) && quad_list[i].arg2[0] == '\0') {
             char* var_to_prop = quad_list[i].result;
             char* const_val = quad_list[i].arg1;
             if (strlen(var_to_prop) == 0) continue; // Cannot propagate if result is empty

             for (int j = i + 1; j < quad_count; j++) {
                 bool propagated_here = false;
                 // Propagate into arg1 of arithmetic ops
                 if ((strcmp(quad_list[j].op, "+") == 0 || strcmp(quad_list[j].op, "-") == 0 ||
                      strcmp(quad_list[j].op, "*") == 0 || strcmp(quad_list[j].op, "/") == 0) &&
                     strcmp(quad_list[j].arg1, var_to_prop) == 0)
                 {
                     strcpy(quad_list[j].arg1, const_val); propagated_here = true;
                 }
                 // Propagate into arg2 of arithmetic ops
                 if ((strcmp(quad_list[j].op, "+") == 0 || strcmp(quad_list[j].op, "-") == 0 ||
                      strcmp(quad_list[j].op, "*") == 0 || strcmp(quad_list[j].op, "/") == 0) &&
                      strcmp(quad_list[j].arg2, var_to_prop) == 0)
                 {
                     strcpy(quad_list[j].arg2, const_val); propagated_here = true;
                 }
                  
                 if (propagated_here) {
                     printf("  Propagated '%s = %s' from Quad %d into Quad %d\n", var_to_prop, const_val, i + 1, j + 1);
                      changed_prop = true;
                 }
             }
         }
     }
      if (!changed_prop) printf("  No constant propagation opportunities found (expected for this IR).\n");

 
     printf("  Running follow-up Constant Folding...\n");

     char buffer[MAX_STR]; char orig_arg1[MAX_STR], orig_arg2[MAX_STR], orig_op[20];
     for (int i = 0; i < quad_count; i++) {
         strcpy(orig_arg1, quad_list[i].arg1); strcpy(orig_arg2, quad_list[i].arg2); strcpy(orig_op, quad_list[i].op);
         if ((strcmp(quad_list[i].op, "+") == 0 || strcmp(quad_list[i].op, "-") == 0 ||
              strcmp(quad_list[i].op, "*") == 0 || strcmp(quad_list[i].op, "/") == 0) &&
             isConstant(quad_list[i].arg1) && isConstant(quad_list[i].arg2)) {
             int c1 = atoi(quad_list[i].arg1); int c2 = atoi(quad_list[i].arg2);
             if (strcmp(quad_list[i].op, "/") == 0 && c2 == 0) { continue; }
             int result_val = evaluateExpression(quad_list[i].op, c1, c2); sprintf(buffer, "%d", result_val);
             printf("    Folding Quad %d (%s = %s %s %s) -> (%s = %s)\n", i + 1, quad_list[i].result, orig_arg1, orig_op, orig_arg2, quad_list[i].result, buffer);
             strcpy(quad_list[i].op, "="); strcpy(quad_list[i].arg1, buffer); quad_list[i].arg2[0] = '\0';
             changed_fold = true;
         }
     }
     if (!changed_fold) printf("    No further folding opportunities found.\n");
     printf("----------------------------------------\n");
}


int main(int argc, char *argv[]) {
    const char* quad_filename = "music_score1quads";

    if (argc > 1) {
        quad_filename = argv[1]; 
    }

    printf("--- Quadruple Optimizer ---\n");

    if (!readQuadsFromFile(quad_filename)) {
        fprintf(stderr, "Failed to read quadruples from %s. Exiting.\n", quad_filename);
        return 1;
    }
    printQuads("Initial Quads Loaded", false);

    int option = 0;
    while (option < 1 || option > 3) {
        printf("\nSelect optimization technique:\n");
        printf("  1. Common Subexpression Elimination (CSE)\n");
        printf("  2. Constant Folding\n");
        printf("  3. Constant Propagation (includes Folding)\n");
        printf("Enter your choice (1-3): ");
        if (scanf("%d", &option) != 1 || option < 1 || option > 3) {
            printf("Invalid choice. Please enter 1, 2, or 3.\n");
            while (getchar() != '\n');
            option = 0;
        }
    }

    switch(option) {
        case 1:
            commonSubexpressionElimination();
            break;
        case 2:
            constantFoldingOptimization();
            break;
        case 3:
            constantPropagationOptimization();
            break;
    }

    printQuads("Optimized Quadruples", false);
    return 0;
}
