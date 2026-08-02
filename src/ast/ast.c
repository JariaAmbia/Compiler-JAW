#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

/* ---------- generic allocator ---------- */

ASTNode *newNode(NodeType type)
{
    ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
    if (!n)
    {
        fprintf(stderr, "Fatal: out of memory while building AST\n");
        exit(1);
    }

    n->type    = type;
    n->strval  = NULL;
    n->numval  = 0.0;
    n->boolval = 0;
    n->left    = NULL;
    n->right   = NULL;
    n->third   = NULL;
    n->next    = NULL;

    return n;
}

static char *dupStr(const char *s)
{
    if (!s) return NULL;
    char *copy = strdup(s);
    if (!copy)
    {
        fprintf(stderr, "Fatal: out of memory while duplicating string\n");
        exit(1);
    }
    return copy;
}

/* ---------- leaf nodes ---------- */

ASTNode *newNumber(double val)
{
    ASTNode *n = newNode(NODE_NUMBER);
    n->numval = val;
    return n;
}

ASTNode *newId(const char *name)
{
    ASTNode *n = newNode(NODE_ID);
    n->strval = dupStr(name);
    return n;
}

ASTNode *newBool(int val)
{
    ASTNode *n = newNode(NODE_BOOL);
    n->boolval = val;
    return n;
}

/* ---------- expressions ---------- */

ASTNode *newBinOp(const char *op, ASTNode *left, ASTNode *right)
{
    ASTNode *n = newNode(NODE_BINOP);
    n->strval = dupStr(op);
    n->left   = left;
    n->right  = right;
    return n;
}

ASTNode *newUnOp(const char *op, ASTNode *operand)
{
    ASTNode *n = newNode(NODE_UNOP);
    n->strval = dupStr(op);
    n->left   = operand;
    return n;
}

/* ---------- statements ---------- */

ASTNode *newDecl(const char *typeName, const char *varName)
{
    ASTNode *n = newNode(NODE_DECL);
    n->strval = dupStr(typeName); /* declared type   */
    n->left   = newId(varName);  /* variable name   */
    return n;
}

ASTNode *newDeclInit(const char *typeName, const char *varName, ASTNode *initExpr)
{
    ASTNode *n = newNode(NODE_DECL_INIT);
    n->strval = dupStr(typeName);
    n->left   = newId(varName);
    n->right  = initExpr;
    return n;
}

ASTNode *newAssign(const char *varName, ASTNode *expr)
{
    ASTNode *n = newNode(NODE_ASSIGN);
    n->left   = newId(varName);
    n->right  = expr;
    return n;
}

ASTNode *newIf(ASTNode *cond, ASTNode *body)
{
    ASTNode *n = newNode(NODE_IF);
    n->left  = cond;
    n->right = body;
    return n;
}

ASTNode *newIfElse(ASTNode *cond, ASTNode *body, ASTNode *elseBody)
{
    ASTNode *n = newNode(NODE_IF);
    n->left  = cond;
    n->right = body;
    n->third = elseBody;
    return n;
}

ASTNode *newWhile(ASTNode *cond, ASTNode *body)
{
    ASTNode *n = newNode(NODE_WHILE);
    n->left  = cond;
    n->right = body;
    return n;
}

ASTNode *newPrint(ASTNode *expr)
{
    ASTNode *n = newNode(NODE_PRINT);
    n->left = expr;
    return n;
}

ASTNode *newProgram(ASTNode *stmtList)
{
    ASTNode *n = newNode(NODE_PROGRAM);
    n->left = stmtList;
    return n;
}

/* ---------- statement-list chaining ---------- */

ASTNode *appendStmt(ASTNode *list, ASTNode *stmt)
{
    if (!list) return stmt;

    ASTNode *cur = list;
    while (cur->next) cur = cur->next;
    cur->next = stmt;

    return list;
}

/* ---------- pretty printer (drawn as an ASCII tree) ---------- */
/*
 * prefix accumulates the "│   " / "    " columns of ancestors that are
 * still open (i.e. have more siblings coming after the current branch),
 * so the tree lines up correctly no matter how deep it gets.
 */

static void nodeLabel(ASTNode *node, char *buf, size_t bufsz)
{
    switch (node->type)
    {
        case NODE_PROGRAM:
            snprintf(buf, bufsz, "Program");
            break;
        case NODE_DECL:
            snprintf(buf, bufsz, "Decl (%s %s)", node->strval, node->left->strval);
            break;
        case NODE_DECL_INIT:
            snprintf(buf, bufsz, "DeclInit (%s %s =)", node->strval, node->left->strval);
            break;
        case NODE_ASSIGN:
            snprintf(buf, bufsz, "Assign (%s =)", node->left->strval);
            break;
        case NODE_IF:
            snprintf(buf, bufsz, "If");
            break;
        case NODE_WHILE:
            snprintf(buf, bufsz, "While");
            break;
        case NODE_PRINT:
            snprintf(buf, bufsz, "Print");
            break;
        case NODE_BINOP:
            snprintf(buf, bufsz, "BinOp (%s)", node->strval);
            break;
        case NODE_UNOP:
            snprintf(buf, bufsz, "UnOp (%s)", node->strval);
            break;
        case NODE_NUMBER:
            snprintf(buf, bufsz, "Number (%g)", node->numval);
            break;
        case NODE_ID:
            snprintf(buf, bufsz, "Id (%s)", node->strval);
            break;
        case NODE_BOOL:
            snprintf(buf, bufsz, "Bool (%s)", node->boolval ? "true" : "false");
            break;
        default:
            snprintf(buf, bufsz, "<unknown node>");
            break;
    }
}

#define BRANCH_MID  "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 "  /* "├── " */
#define BRANCH_LAST "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 "  /* "└── " */
#define PIPE_COL    "\xe2\x94\x82   "                        /* "│   " */
#define BLANK_COL   "    "

static void printStmtList(ASTNode *list, const char *prefix);

/* Prints a single labelled child edge (e.g. "Cond:" / "Body:") followed by
 * the subtree/statement-list rooted at `child`.
 */
static void printLabelledChild(const char *label, ASTNode *child,
                                const char *prefix, int isLast)
{
    printf("%s%s%s:\n", prefix, isLast ? BRANCH_LAST : BRANCH_MID, label);

    char childPrefix[256];
    snprintf(childPrefix, sizeof(childPrefix), "%s%s", prefix,
             isLast ? BLANK_COL : PIPE_COL);

    printStmtList(child, childPrefix);
}

/* Prints one node (never follows ->next) plus its own subtree. */
static void printNode(ASTNode *node, const char *prefix, int isLast)
{
    char label[64];
    nodeLabel(node, label, sizeof(label));

    printf("%s%s%s\n", prefix, isLast ? BRANCH_LAST : BRANCH_MID, label);

    char childPrefix[256];
    snprintf(childPrefix, sizeof(childPrefix), "%s%s", prefix,
             isLast ? BLANK_COL : PIPE_COL);

    switch (node->type)
    {
        case NODE_PROGRAM:
            printStmtList(node->left, childPrefix);
            break;

        case NODE_DECL_INIT:
        case NODE_ASSIGN:
            printNode(node->right, childPrefix, 1);
            break;

        case NODE_WHILE:
            printLabelledChild("Cond", node->left, childPrefix, 0);
            printLabelledChild("Body", node->right, childPrefix, 1);
            break;

        case NODE_IF:
            if (node->third) {
                printLabelledChild("Cond", node->left, childPrefix, 0);
                printLabelledChild("Body", node->right, childPrefix, 0);
                printLabelledChild("Else", node->third, childPrefix, 1);
            } else {
                printLabelledChild("Cond", node->left, childPrefix, 0);
                printLabelledChild("Body", node->right, childPrefix, 1);
            }
            break;

        case NODE_PRINT:
            printNode(node->left, childPrefix, 1);
            break;

        case NODE_BINOP:
            printNode(node->left, childPrefix, 0);
            printNode(node->right, childPrefix, 1);
            break;

        case NODE_UNOP:
            printNode(node->left, childPrefix, 1);
            break;

        default:
            break; /* leaves: NODE_DECL, NODE_NUMBER, NODE_ID, NODE_BOOL */
    }
}

/* Prints every statement in a sibling chain (linked via ->next), each as
 * its own branch, so a whole block/body renders as a proper tree.
 */
static void printStmtList(ASTNode *list, const char *prefix)
{
    if (!list) return;

    int count = 0;
    for (ASTNode *cur = list; cur; cur = cur->next) count++;

    int i = 0;
    for (ASTNode *cur = list; cur; cur = cur->next, i++)
        printNode(cur, prefix, i == count - 1);
}

/* Public entry point. `depth` is kept for API compatibility with the rest
 * of the project; the tree is always rendered from a clean left margin,
 * and a full statement chain (siblings linked via ->next) is handled.
 */
void printAST(ASTNode *node, int depth)
{
    (void)depth;
    printStmtList(node, "");
}

/* ---------- cleanup ---------- */

void freeAST(ASTNode *node)
{
    if (!node) return;

    freeAST(node->left);
    freeAST(node->right);
    freeAST(node->third);
    freeAST(node->next);

    if (node->strval) free(node->strval);
    free(node);
}