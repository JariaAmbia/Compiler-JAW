%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "../ast/ast.h"
#include "../../tac.h"

extern int yylex();
extern int line;

void yyerror(const char *s);

ASTNode *root = NULL;
%}

%code requires {
#include "../ast/ast.h"
}

%union{
    ASTNode *node;
    char *str;
    int value;
}

%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE

%token <str> ID
%token <str> NUMBER

%token EQ NE LE GE
%token AND OR NOT

%type <node> program
%type <node> statements
%type <node> statement
%type <node> declaration
%type <node> assignment
%type <node> expr
%type <node> print_statement

%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%right NOT

%%

program
    : statements
    {
        root = $1;

        printf("\n========== ABSTRACT SYNTAX TREE ==========\n");
        printAST(root, 0);

        printf("\n========== THREE ADDRESS CODE ==========\n");
        generateASTTAC(root);
    }
;
statements
    : statements statement
    {
        $$ = createNode("Program");

        addChild($$, $1);
        addChild($$, $2);
    }

    | statement
    {
        $$ = createNode("Program");

        addChild($$, $1);
    }
;
statement
    : declaration
    {
        $$ = $1;
    }

    | assignment
    {
        $$ = $1;
    }

    | print_statement
    {
        $$ = $1;
    }
;


declaration
    : INT ID ';'
    {
        insertSymbol($2, TYPE_INT);

        $$ = createNode("Declaration");

        addChild($$, createNode("int"));
        addChild($$, createNode($2));
    }

    | FLOAT ID ';'
    {
        insertSymbol($2, TYPE_FLOAT);

        $$ = createNode("Declaration");

        addChild($$, createNode("float"));
        addChild($$, createNode($2));
    }

    | BOOL ID ';'
    {
        insertSymbol($2, TYPE_BOOL);

        $$ = createNode("Declaration");

        addChild($$, createNode("bool"));
        addChild($$, createNode($2));
    }
;
assignment
    : ID '=' expr ';'
    {
        $$ = createNode("Assignment");

        addChild($$, createNode($1));
        addChild($$, $3);

        generateTAC("=", $3->name, "", $1);
    }
;

expr
    : NUMBER
    {
        $$ = createNode($1);
    }

    | ID
    {
        $$ = createNode($1);
    }

    | expr '+' expr
    {
        $$ = createNode("+");

        addChild($$, $1);
        addChild($$, $3);

        char *temp = newTemp();

        generateTAC("+",
                    $1->name,
                    $3->name,
                    temp);

        strcpy($$->name, temp);

        free(temp);
    }

    | expr '-' expr
    {
        $$ = createNode("-");

        addChild($$, $1);
        addChild($$, $3);

        char *temp = newTemp();

        generateTAC("-",
                    $1->name,
                    $3->name,
                    temp);

        strcpy($$->name, temp);

        free(temp);
    }

    | expr '*' expr
    {
        $$ = createNode("*");

        addChild($$, $1);
        addChild($$, $3);

        char *temp = newTemp();

        generateTAC("*",
                    $1->name,
                    $3->name,
                    temp);

        strcpy($$->name, temp);

        free(temp);
    }

    | expr '/' expr
    {
        $$ = createNode("/");

        addChild($$, $1);
        addChild($$, $3);

        char *temp = newTemp();

        generateTAC("/",
                    $1->name,
                    $3->name,
                    temp);

        strcpy($$->name, temp);

        free(temp);
    }

    | '(' expr ')'
    {
        $$ = $2;
    }
;
/* Print Statement */

print_statement
    : PRINT '(' ID ')' ';'
    {
        ASTNode *idNode = createNode($3);

        $$ = createNode("Print");

        addChild($$, idNode);

        generatePrintTAC(idNode);
    }
;

%%
 
void yyerror(const char *s)
{
    fprintf(stderr,
            "Syntax Error: %s at line %d\n",
            s,
            line);
}

int main()
{
    printf("Starting Compiler...\n");
    yyparse();
    return 0;
}