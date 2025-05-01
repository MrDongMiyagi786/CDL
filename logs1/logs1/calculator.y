%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int flag = 0;

int yylex(void);
void yyerror(const char *s);

typedef struct Node {
    char label[50];
    struct Node *left, *right;
    int val;
} Node;

// Corrected function declaration
Node* createNode(const char *label, Node* left, Node* right);
void printParseTree(Node* root, int depth);
void freeTree(Node* root);

Node* root = NULL;
%}

%union {
    int val;
    struct Node* node;
}

%token <val> NUMBER
%type  <node> E

%left '+' '-'
%left '*' '/' '%'
%left '(' ')'

%%

ArithmeticExpression:
    E {
        root = $1;
    }
;

E:
      E '+' E {
          $$ = createNode("+", $1, $3);
          $$->val = $1->val + $3->val;
      }
    | E '-' E {
          $$ = createNode("-", $1, $3);
          $$->val = $1->val - $3->val;
      }
    | E '*' E {
          $$ = createNode("*", $1, $3);
          $$->val = $1->val * $3->val;
      }
    | E '/' E {
          $$ = createNode("/", $1, $3);
          $$->val = $1->val / $3->val;
      }
    | E '%' E {
          $$ = createNode("%", $1, $3);
          $$->val = $1->val % $3->val;
      }
    | '(' E ')' {
          $$ = createNode("()", $2, NULL);
          $$->val = $2->val;
      }
    | NUMBER {
          char buffer[20];
          sprintf(buffer, "%d", $1);
          $$ = createNode(buffer, NULL, NULL);
          $$->val = $1;
      }
;

%%

Node* createNode(const char *label, Node* left, Node* right) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    strcpy(newNode->label, label);
    newNode->left = left;
    newNode->right = right;
    newNode->val = 0;
    return newNode;
}

void printParseTree(Node* root, int depth) {
    if (!root) return;

    for (int i = 0; i < depth; i++)
        printf("  ");

    printf("|-- %s\n", root->label);

    printParseTree(root->left, depth + 1);
    printParseTree(root->right, depth + 1);
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void yyerror(const char *s) {
    printf("\nEntered arithmetic expression is Invalid\n\n");
    flag = 1;
}

int main() {
    printf("Enter Any Arithmetic Expression which can have operations Addition, Subtraction, Multiplication, Division, Modulus and Round brackets:\n");

    yyparse();

    if (flag == 0 && root != NULL) {
        printf("\nResult = %d\n", root->val);
        printf("\nEntered arithmetic expression is Valid\n");
        printf("\nParse Tree\n===========================================\n");
        printParseTree(root, 0);
    }

    freeTree(root);
    return 0;
}