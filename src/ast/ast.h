#ifndef AST_H
#define AST_H

typedef struct ASTNode
{
    char name[50];

    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;

} ASTNode;


ASTNode* createNode(char *name);

ASTNode* createOperatorNode(
        char *op,
        ASTNode *left,
        ASTNode *right
);

void printAST(ASTNode *root,int level);


#endif