#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100

/* Data Types */
typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL
} DataType;

/* Symbol Structure */
typedef struct
{
    char name[50];
    DataType type;
} Symbol;

/* Function Prototypes */

void initSymbolTable();

int insertSymbol(char *name, DataType type);

int searchSymbol(char *name);

void printSymbolTable();

#endif