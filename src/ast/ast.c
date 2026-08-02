#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "ast.h"


ASTNode* createNode(char *name)
{
    ASTNode *node;

    node=(ASTNode*)malloc(sizeof(ASTNode));

    strcpy(node->name,name);

    node->left=NULL;
    node->right=NULL;
    node->third=NULL;

    return node;
}



ASTNode* createOperatorNode(
        char *op,
        ASTNode *left,
        ASTNode *right
)
{
    ASTNode *node=createNode(op);

    node->left=left;
    node->right=right;

    return node;
}



/*
   Walks the "third" chain as siblings at the SAME indentation level
   (used for statement lists), and recurses into left/right for
   deeper structure (operands, condition/body, etc).
*/
void printAST(ASTNode *root,int level)
{
    for( ; root != NULL; root = root->third)
    {
        for(int i=0;i<level;i++)
            printf("    ");

        printf("|-- %s\n",root->name);

        printAST(root->left,level+1);

        printAST(root->right,level+1);
    }
}