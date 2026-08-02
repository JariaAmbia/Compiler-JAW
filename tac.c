#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tac.h"

/* ---------- instruction list (module-private) ---------- */

static TACInstr *head = NULL;
static TACInstr *tail = NULL;

static int tempCount  = 0;   /* for t1, t2, t3, ...   */
static int labelCount = 0;   /* for L1, L2, L3, ...   */

/* ---------- small helpers ---------- */

static char *dupStr(const char *s)
{
    char *c = strdup(s);
    if (!c)
    {
        fprintf(stderr, "Fatal: out of memory in TAC generator\n");
        exit(1);
    }
    return c;
}

/* returns a freshly allocated string "t<n>" for a new temporary variable */
static char *newTemp(void)
{
    char buf[16];
    tempCount++;
    snprintf(buf, sizeof(buf), "t%d", tempCount);
    return dupStr(buf);
}

/* returns a freshly allocated string "L<n>" for a new label */
static char *newLabel(void)
{
    char buf[16];
    labelCount++;
    snprintf(buf, sizeof(buf), "L%d", labelCount);
    return dupStr(buf);
}

/* formats a numeric literal without a needless trailing ".0" */
static char *numToStr(double val)
{
    char buf[64];
    if (val == (long long)val)
        snprintf(buf, sizeof(buf), "%lld", (long long)val);
    else
        snprintf(buf, sizeof(buf), "%g", val);
    return dupStr(buf);
}

/* appends one instruction to the global list */
static void emit(const char *op, const char *arg1, const char *arg2, const char *result)
{
    TACInstr *instr = (TACInstr *)malloc(sizeof(TACInstr));
    if (!instr)
    {
        fprintf(stderr, "Fatal: out of memory in TAC generator\n");
        exit(1);
    }

    instr->op     = op     ? dupStr(op)     : NULL;
    instr->arg1   = arg1   ? dupStr(arg1)   : NULL;
    instr->arg2   = arg2   ? dupStr(arg2)   : NULL;
    instr->result = result ? dupStr(result) : NULL;
    instr->next   = NULL;

    if (!head) { head = tail = instr; }
    else       { tail->next = instr; tail = instr; }
}

/* ---------- expression code generation ---------- */
/* Returns a string "place" (variable name, temp name, or literal) that
 * holds the value of the expression. Caller does NOT own/free this string;
 * it is either borrowed from the AST node or a newly emitted temp name
 * that stays referenced only inside emitted instructions.
 */
static char *genExpr(ASTNode *node)
{
    if (!node) return dupStr("");

    switch (node->type)
    {
        case NODE_NUMBER:
            return numToStr(node->numval);

        case NODE_BOOL:
            return dupStr(node->boolval ? "true" : "false");

        case NODE_ID:
            return dupStr(node->strval);

        case NODE_BINOP:
        {
            char *l = genExpr(node->left);
            char *r = genExpr(node->right);
            char *t = newTemp();
            emit(node->strval, l, r, t);
            free(l);
            free(r);
            return t;
        }

        case NODE_UNOP:
        {
            char *operand = genExpr(node->left);
            char *t = newTemp();
            emit(node->strval, operand, NULL, t);
            free(operand);
            return t;
        }

        default:
            return dupStr("?");
    }
}

/* ---------- statement code generation ---------- */
/* Walks a statement chain (node->next links siblings in the same block). */
static void genStmt(ASTNode *node)
{
    while (node)
    {
        switch (node->type)
        {
            case NODE_DECL:
                /* pure declarations carry no runtime code */
                break;

            case NODE_DECL_INIT:
            {
                char *val = genExpr(node->right);
                emit("=", val, NULL, node->left->strval);
                free(val);
                break;
            }

            case NODE_ASSIGN:
            {
                char *val = genExpr(node->right);
                emit("=", val, NULL, node->left->strval);
                free(val);
                break;
            }

            case NODE_IF:
            {
                char *cond = genExpr(node->left);

                if (node->third)
                {
                    /* if-else: two labels are needed --
                     *   ifFalse cond goto Lelse
                     *   <then body>
                     *   goto Lend
                     * Lelse:
                     *   <else body>
                     * Lend:
                     */
                    char *Lelse = newLabel();
                    char *Lend  = newLabel();

                    emit("ifFalse", cond, NULL, Lelse);
                    free(cond);

                    genStmt(node->right);   /* then-body */
                    emit("goto", NULL, NULL, Lend);

                    emit("label", NULL, NULL, Lelse);
                    genStmt(node->third);   /* else-body */

                    emit("label", NULL, NULL, Lend);

                    free(Lelse);
                    free(Lend);
                }
                else
                {
                    /* plain if: a single label marks the fall-through point */
                    char *Lfalse = newLabel();

                    emit("ifFalse", cond, NULL, Lfalse);
                    free(cond);

                    genStmt(node->right);   /* if-body */

                    emit("label", NULL, NULL, Lfalse);
                    free(Lfalse);
                }
                break;
            }

            case NODE_WHILE:
            {
                char *Lstart = newLabel();
                char *Lend   = newLabel();

                emit("label", NULL, NULL, Lstart);

                char *cond = genExpr(node->left);
                emit("ifFalse", cond, NULL, Lend);
                free(cond);

                genStmt(node->right);   /* while-body */

                emit("goto", NULL, NULL, Lstart);
                emit("label", NULL, NULL, Lend);

                free(Lstart);
                free(Lend);
                break;
            }

            case NODE_PRINT:
            {
                char *val = genExpr(node->left);
                emit("print", val, NULL, NULL);
                free(val);
                break;
            }

            default:
                break;
        }

        node = node->next;
    }
}

/* ---------- public API ---------- */

void generateTAC(ASTNode *root)
{
    head = tail = NULL;
    tempCount = labelCount = 0;

    if (!root) return;

    if (root->type == NODE_PROGRAM)
        genStmt(root->left);
    else
        genStmt(root);
}

TACInstr *getTACList(void)
{
    return head;
}

void printTAC(void)
{
    int idx = 0;
    for (TACInstr *i = head; i != NULL; i = i->next)
    {
        printf("%3d: ", idx++);

        if (strcmp(i->op, "label") == 0)
        {
            printf("%s:\n", i->result);
        }
        else if (strcmp(i->op, "goto") == 0)
        {
            printf("goto %s\n", i->result);
        }
        else if (strcmp(i->op, "ifFalse") == 0)
        {
            printf("ifFalse %s goto %s\n", i->arg1, i->result);
        }
        else if (strcmp(i->op, "print") == 0)
        {
            printf("print %s\n", i->arg1);
        }
        else if (strcmp(i->op, "=") == 0)
        {
            printf("%s = %s\n", i->result, i->arg1);
        }
        else if (i->arg2 != NULL)
        {
            /* binary op: result = arg1 op arg2 */
            printf("%s = %s %s %s\n", i->result, i->arg1, i->op, i->arg2);
        }
        else
        {
            /* unary op: result = op arg1 */
            printf("%s = %s%s\n", i->result, i->op, i->arg1);
        }
    }
}

void freeTACList(void)
{
    TACInstr *cur = head;
    while (cur)
    {
        TACInstr *nxt = cur->next;
        free(cur->op);
        free(cur->arg1);
        free(cur->arg2);
        free(cur->result);
        free(cur);
        cur = nxt;
    }
    head = tail = NULL;
}