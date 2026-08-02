%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "../ast/ast.h"
#include "../../tac.h"

int yylex(void);
void yyerror(const char *s);
%}

%union{
    char   *str;
    double  num;
    ASTNode *node;
}

/* Tokens */
%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE
%token <str> ID
%token <num> NUMBER
%token EQ NE LE GE
%token AND OR NOT

%type <node> program statements statement declaration assignment
%type <node> if_statement while_statement print_statement expr

/* Operator precedence */
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%right NOT

%%

program:
      statements
        {
            ASTNode *root = newProgram($1);

            printf("Program Parsed Successfully\n");

            printf("\n========== ABSTRACT SYNTAX TREE ==========\n");
            printAST(root, 0);

            printf("\n========== THREE ADDRESS CODE =============\n");
            generateTAC(root);
            printTAC();
            printf("=============================================\n");

            freeTACList();
            freeAST(root);
        }
      ;

statements:
      statements statement
        { $$ = appendStmt($1, $2); }
    | statement
        { $$ = $1; }
    ;

statement:
      declaration      { $$ = $1; }
    | assignment        { $$ = $1; }
    | if_statement       { $$ = $1; }
    | while_statement    { $$ = $1; }
    | print_statement    { $$ = $1; }
    ;

declaration:
      INT ID ';'
      {
          insertSymbol($2, TYPE_INT);
          printf("Declaration Found\n");
          $$ = newDecl("int", $2);
      }

    | FLOAT ID ';'
      {
          insertSymbol($2, TYPE_FLOAT);
          printf("Declaration Found\n");
          $$ = newDecl("float", $2);
      }

    | BOOL ID ';'
      {
          insertSymbol($2, TYPE_BOOL);
          printf("Declaration Found\n");
          $$ = newDecl("bool", $2);
      }

    | INT ID '=' expr ';'
      {
          insertSymbol($2, TYPE_INT);
          printf("Initialized Declaration Found\n");
          $$ = newDeclInit("int", $2, $4);
      }

    | FLOAT ID '=' expr ';'
      {
          insertSymbol($2, TYPE_FLOAT);
          printf("Initialized Declaration Found\n");
          $$ = newDeclInit("float", $2, $4);
      }

    | BOOL ID '=' expr ';'
      {
          insertSymbol($2, TYPE_BOOL);
          printf("Initialized Declaration Found\n");
          $$ = newDeclInit("bool", $2, $4);
      }
    ;

assignment:
      ID '=' expr ';'
      {
          if(searchSymbol($1) == -1)
          {
              printf("Semantic Error: Variable '%s' not declared.\n", $1);
          }
          else
          {
              printf("Assignment Found\n");
          }
          $$ = newAssign($1, $3);
      }
    ;

if_statement:
      IF '(' expr ')' '{' statements '}'
      {
          printf("If Statement Found\n");
          $$ = newIf($3, $6);
      }
    | IF '(' expr ')' '{' statements '}' ELSE '{' statements '}'
      {
          printf("If-Else Statement Found\n");
          $$ = newIfElse($3, $6, $10);
      }
    ;

while_statement:
      WHILE '(' expr ')' '{' statements '}'
      {
          printf("While Statement Found\n");
          $$ = newWhile($3, $6);
      }
    ;

print_statement:
      PRINT '(' expr ')' ';'
        {
            printf("Print Statement Found\n");
            $$ = newPrint($3);
        }
    ;

expr:
      expr '+' expr   { $$ = newBinOp("+", $1, $3); }
    | expr '-' expr   { $$ = newBinOp("-", $1, $3); }
    | expr '*' expr   { $$ = newBinOp("*", $1, $3); }
    | expr '/' expr   { $$ = newBinOp("/", $1, $3); }

    | expr '<' expr   { $$ = newBinOp("<", $1, $3); }
    | expr '>' expr   { $$ = newBinOp(">", $1, $3); }
    | expr LE expr    { $$ = newBinOp("<=", $1, $3); }
    | expr GE expr    { $$ = newBinOp(">=", $1, $3); }
    | expr EQ expr    { $$ = newBinOp("==", $1, $3); }
    | expr NE expr    { $$ = newBinOp("!=", $1, $3); }

    | expr AND expr   { $$ = newBinOp("&&", $1, $3); }
    | expr OR expr    { $$ = newBinOp("||", $1, $3); }
    | NOT expr        { $$ = newUnOp("!", $2); }

    | '(' expr ')'    { $$ = $2; }

    | NUMBER          { $$ = newNumber($1); }
    | ID              { $$ = newId($1); }
    | TRUE            { $$ = newBool(1); }
    | FALSE           { $$ = newBool(0); }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax Error: %s\n", s);
}

int main()
{
    printf("Parsing Started...\n");
    initSymbolTable();
    yyparse();
    printSymbolTable();
    printf("Parsing Finished.\n");

    return 0;
}
