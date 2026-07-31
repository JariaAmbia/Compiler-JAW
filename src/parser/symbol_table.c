#include <stdio.h>
#include <string.h>

#include "symbol_table.h"

Symbol table[MAX_SYMBOLS];
int symbolCount = 0;

/* Initialize Symbol Table */

void initSymbolTable()
{
    symbolCount = 0;
}

/* Search Symbol */

int searchSymbol(char *name)
{
    int i;

    for(i = 0; i < symbolCount; i++)
    {
        if(strcmp(table[i].name, name) == 0)
            return i;
    }

    return -1;
}

/* Insert Symbol */

int insertSymbol(char *name, DataType type)
{
    if(searchSymbol(name) != -1)
    {
        printf("Semantic Error: Variable '%s' already declared.\n", name);
        return 0;
    }

    strcpy(table[symbolCount].name, name);
    table[symbolCount].type = type;

    symbolCount++;

    return 1;
}

/* Print Symbol Table */

void printSymbolTable()
{
    int i;

    printf("\n========== SYMBOL TABLE ==========\n");

    printf("%-15s %-10s\n", "Identifier", "Type");

    printf("----------------------------------\n");

    for(i = 0; i < symbolCount; i++)
    {
        printf("%-15s ", table[i].name);

        switch(table[i].type)
        {
            case TYPE_INT:
                printf("INT");
                break;

            case TYPE_FLOAT:
                printf("FLOAT");
                break;

            case TYPE_BOOL:
                printf("BOOL");
                break;
        }

        printf("\n");
    }

    printf("==================================\n");
}