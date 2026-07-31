%{
#include <stdio.h>
#include <stdlib.h>
#include "src/parser/symbol_table.h"
int yylex(void);
void yyerror(const char *s);
%}
%union{
    char *str;
}

/* Tokens */
%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE
%token <str> ID
%token NUMBER
%token EQ NE LE GE
%token AND OR NOT

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
        { printf("Program Parsed Successfully\n"); }
      ;

statements:
      statements statement
    | statement
    ;

statement:
      declaration
    | assignment
    | if_statement
    | while_statement
    | print_statement
    ;

declaration:
      INT ID ';'
      {
           insertSymbol($2, TYPE_INT);
          printf("Declaration Found\n");
      }

    | FLOAT ID ';'
      {  
          insertSymbol($2, TYPE_FLOAT);
          printf("Declaration Found\n");
      }

    | BOOL ID ';'
      {
          insertSymbol($2, TYPE_BOOL);
          printf("Declaration Found\n");
      }

    | INT ID '=' expr ';'
      {
          printf("Initialized Declaration Found\n");
      }

    | FLOAT ID '=' expr ';'
      {
          printf("Initialized Declaration Found\n");
      }

    | BOOL ID '=' expr ';'
      {
          printf("Initialized Declaration Found\n");
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
      }
    ;

if_statement:
      IF '(' expr ')' '{' statements '}'
      {
          printf("If Statement Found\n");
      }
    ;

while_statement:
      WHILE '(' expr ')' '{' statements '}'
      {
          printf("While Statement Found\n");
      }
    ;

print_statement:
      PRINT '(' expr ')' ';'
        { printf("Print Statement Found\n"); }
    ;
expr:
      expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr

    | expr '<' expr
    | expr '>' expr
    | expr LE expr
    | expr GE expr
    | expr EQ expr
    | expr NE expr

    | expr AND expr
    | expr OR expr
    | NOT expr

    | '(' expr ')'

    | NUMBER
    | ID
    | TRUE
    | FALSE
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