#ifndef AST_H
#define AST_H

/* Kinds of AST nodes produced by the parser */
typedef enum {
    NODE_PROGRAM,     /* root: holds the top-level statement list       */
    NODE_STMT_LIST,   /* internal chaining node (rarely inspected)      */
    NODE_DECL,        /* plain declaration:      int x;                 */
    NODE_DECL_INIT,   /* initialized declaration: int x = expr;         */
    NODE_ASSIGN,      /* x = expr;                                      */
    NODE_IF,          /* if (cond) { body }                             */
    NODE_WHILE,       /* while (cond) { body }                          */
    NODE_PRINT,       /* print(expr);                                   */
    NODE_BINOP,       /* expr op expr                                   */
    NODE_UNOP,        /* op expr   (currently only NOT)                 */
    NODE_NUMBER,      /* numeric literal                                */
    NODE_ID,          /* identifier reference                           */
    NODE_BOOL         /* true / false literal                           */
} NodeType;

typedef struct ASTNode {
    NodeType type;

    char   *strval;   /* identifier name, declared type name, or op symbol */
    double  numval;   /* value for NODE_NUMBER                             */
    int     boolval;  /* value for NODE_BOOL                               */

    struct ASTNode *left;   /* generic first child  (e.g. LHS, cond, expr)  */
    struct ASTNode *right;  /* generic second child (e.g. RHS, body)        */
    struct ASTNode *third;  /* reserved for future use (e.g. else-branch)   */

    struct ASTNode *next;   /* sibling pointer, used to chain statements    */
} ASTNode;

/* --- constructors --- */
ASTNode *newNode(NodeType type);
ASTNode *newNumber(double val);
ASTNode *newId(const char *name);
ASTNode *newBool(int val);
ASTNode *newBinOp(const char *op, ASTNode *left, ASTNode *right);
ASTNode *newUnOp(const char *op, ASTNode *operand);
ASTNode *newDecl(const char *typeName, const char *varName);
ASTNode *newDeclInit(const char *typeName, const char *varName, ASTNode *initExpr);
ASTNode *newAssign(const char *varName, ASTNode *expr);
ASTNode *newIf(ASTNode *cond, ASTNode *body);
ASTNode *newIfElse(ASTNode *cond, ASTNode *body, ASTNode *elseBody);
ASTNode *newWhile(ASTNode *cond, ASTNode *body);
ASTNode *newPrint(ASTNode *expr);
ASTNode *newProgram(ASTNode *stmtList);

/* append `stmt` to the end of `list` (list may be NULL) and return the head */
ASTNode *appendStmt(ASTNode *list, ASTNode *stmt);

/* --- utilities --- */
void printAST(ASTNode *node, int depth);   /* pretty-print the tree to stdout   */
void freeAST(ASTNode *node);               /* release all nodes                 */

#endif /* AST_H */