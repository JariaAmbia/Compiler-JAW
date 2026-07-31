#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tac.h"

static int tempCount = 0;

/* Generate temporary variable */
char *newTemp()
{
    char *temp = (char *)malloc(20);

    if (temp == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    sprintf(temp, "t%d", tempCount++);

    return temp;
}

/* Generate Three Address Code */
void generateTAC(
    char *op,
    char *arg1,
    char *arg2,
    char *result)
{
    if (strcmp(op, "=") == 0)
    {
        printf("%s = %s\n", result, arg1);
    }
    else
    {
        printf("%s = %s %s %s\n",
               result,
               arg1,
               op,
               arg2);
    }
}

/* Generate TAC for print */
void generatePrintTAC(ASTNode *node)
{
    if (node == NULL)
        return;

    printf("print %s\n", node->name);
}

/* Traverse AST and generate TAC */
void generateASTTAC(ASTNode *root)
{
    if (root == NULL)
        return;

    for (int i = 0; i < root->childCount; i++)
    {
        generateASTTAC(root->children[i]);
    }

    if (strcmp(root->name, "Print") == 0)
    {
        if (root->childCount > 0)
        {
            generatePrintTAC(root->children[0]);
        }
    }
}