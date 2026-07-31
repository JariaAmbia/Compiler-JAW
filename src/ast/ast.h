#ifndef AST_H
#define AST_H

#define MAX_CHILDREN 10

typedef struct ASTNode
{
    char name[50];

    struct ASTNode *children[MAX_CHILDREN];

    int childCount;

} ASTNode;


/* Create a new AST node */
ASTNode *createNode(char *name);

/* Add child node */
void addChild(ASTNode *parent, ASTNode *child);

/* Print AST */
void printAST(ASTNode *root, int level);

/* Free AST memory */
void freeAST(ASTNode *root);

#endif