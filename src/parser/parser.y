%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* Tokens */
%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE
%token ID
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
          printf("Declaration Found\n");
      }

    | FLOAT ID ';'
      {
          printf("Declaration Found\n");
      }

    | BOOL ID ';'
      {
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
        { printf("Assignment Found\n"); }
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

    yyparse();

    printf("Parsing Finished.\n");

    return 0;
}