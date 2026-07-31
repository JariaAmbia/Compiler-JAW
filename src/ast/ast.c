#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

/* Create a new AST node */
ASTNode *createNode(char *name)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

    if (node == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    strcpy(node->name, name);

    node->childCount = 0;

    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        node->children[i] = NULL;
    }

    return node;
}

/* Add a child node */
void addChild(ASTNode *parent, ASTNode *child)
{
    if (parent == NULL || child == NULL)
        return;

    if (parent->childCount < MAX_CHILDREN)
    {
        parent->children[parent->childCount] = child;
        parent->childCount++;
    }
}

/* Print AST */
void printAST(ASTNode *root, int level)
{
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    printf("%s\n", root->name);

    for (int i = 0; i < root->childCount; i++)
    {
        printAST(root->children[i], level + 1);
    }
}

/* Free AST memory */
void freeAST(ASTNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i < root->childCount; i++)
    {
        freeAST(root->children[i]);
    }

    free(root);
}