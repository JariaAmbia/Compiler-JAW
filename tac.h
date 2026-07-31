#ifndef TAC_H
#define TAC_H

#include "src/ast/ast.h"

/* Generate temporary variable */
char *newTemp();

/* Generate Three Address Code */
void generateTAC(
    char *op,
    char *arg1,
    char *arg2,
    char *result
);

/* Generate TAC for print statement */
void generatePrintTAC(ASTNode *node);

/* Generate TAC from AST (optional) */
void generateASTTAC(ASTNode *root);

#endif