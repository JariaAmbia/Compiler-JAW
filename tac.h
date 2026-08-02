#ifndef TAC_H
#define TAC_H

#include "ast/ast.h"

/*
 * A single Three-Address-Code instruction, e.g.:
 *
 *   result = arg1 op arg2      (op is "+", "-", "*", "/", "<", "==", ...)
 *   result = arg1              (op is "=")
 *   result = ! arg1            (op is "!"  -- unary)
 *   ifFalse arg1 goto result   (op is "ifFalse", result holds the label)
 *   goto result                (op is "goto",    result holds the label)
 *   result:                    (op is "label",   result holds the label)
 *   print arg1                 (op is "print")
 */
typedef struct TACInstr {
    char *op;
    char *arg1;
    char *arg2;
    char *result;
    struct TACInstr *next;
} TACInstr;

/* Walks the AST and builds the internal TAC instruction list. */
void generateTAC(ASTNode *root);

/* Prints the generated TAC instructions in order. */
void printTAC(void);

/* Frees the internal TAC instruction list. */
void freeTACList(void);

/* Returns the head of the generated instruction list (NULL if none yet). */
TACInstr *getTACList(void);

#endif /* TAC_H */