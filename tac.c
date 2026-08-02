#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tac.h"


int tempCount = 1;


char* newTemp()
{
    char *temp = malloc(20);

    sprintf(temp, "t%d", tempCount++);

    return temp;
}



/*
   Walks the "third" chain (statement lists) as a loop, so every
   statement in a block gets processed - not just the first one.
   "continue" moves to root->third (the for-loop increment) instead
   of exiting the whole function, matching the old "return" behavior
   for a single node while still advancing through siblings.
*/
void generateTAC(ASTNode *root)
{
    for( ; root != NULL; root = root->third)
    {

        /* Arithmetic Expression */

        if(strcmp(root->name, "+") == 0 ||
           strcmp(root->name, "-") == 0 ||
           strcmp(root->name, "*") == 0 ||
           strcmp(root->name, "/") == 0)
        {

            char *temp = newTemp();


            generateTAC(root->left);

            generateTAC(root->right);



            printf("%s = %s %s %s\n",
                   temp,
                   root->left->name,
                   root->name,
                   root->right->name);



            strcpy(root->name,temp);


            continue;
        }



        /* Assignment */

        if(strcmp(root->name,"=")==0)
        {

            generateTAC(root->right);


            printf("%s = %s\n",
                   root->left->name,
                   root->right->name);


            continue;
        }



        /* Print */

        if(strcmp(root->name,"PRINT")==0)
        {

            printf("print %s\n",
                   root->left->name);


            continue;
        }



        generateTAC(root->left);

        generateTAC(root->right);
    }
}