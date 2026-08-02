%{
#include <stdio.h>
#include <stdlib.h>

#include "src/parser/symbol_table.h"
#include "../ast/ast.h"
#include "../../tac.h"

int yylex(void);
void yyerror(const char *s);

extern char *yytext;

ASTNode *root;

%}
%union{
    char *str;
    ASTNode *node;
}
/* Tokens */
%token INT FLOAT BOOL
%token IF ELSE WHILE PRINT
%token TRUE FALSE
%token <str> ID
%token NUMBER
%token EQ NE LE GE
%token AND OR NOT

%type <node> expr
%type <node> assignment
%type <node> declaration
%type <node> statement
%type <node> statements
%type <node> if_statement
%type <node> while_statement
%type <node> print_statement


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
          root = $1;

          printf("\nSyntax Analysis Completed Successfully\n");

          printf("\n========== ABSTRACT SYNTAX TREE ==========\n");
          printAST(root, 0);
          printf("AST Generated Successfully\n");

          printf("\n========== THREE ADDRESS CODE ==========\n");
          generateTAC(root);
          printf("TAC Generated Successfully\n");
      }
;

statements:
      statements statement
      {
          /* Link the new statement onto the end of the list via
             the "third" pointer, so printAST/generateTAC can walk
             all statements in order as siblings at the same level. */
          ASTNode *tail = $1;
          while(tail->third != NULL)
              tail = tail->third;
          tail->third = $2;
          $$ = $1;
      }

    | statement
      {
          $$ = $1;
      }
;
statement:
      declaration       { $$ = $1; }
    | assignment         { $$ = $1; }
    | if_statement        { $$ = $1; }
    | while_statement     { $$ = $1; }
    | print_statement     { $$ = $1; }
    ;

declaration:
      INT ID ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_INT);
          }
          printf("Declaration Found\n");
          $$ = createNode($2);
      }

    | FLOAT ID ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_FLOAT);
          }
          printf("Declaration Found\n");
          $$ = createNode($2);
      }

    | BOOL ID ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_BOOL);
          }
          printf("Declaration Found\n");
          $$ = createNode($2);
      }

    | INT ID '=' expr ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_INT);
          }
          printf("Initialized Declaration Found\n");
          $$ = createOperatorNode("=", createNode($2), $4);
      }

    | FLOAT ID '=' expr ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_FLOAT);
          }
          printf("Initialized Declaration Found\n");
          $$ = createOperatorNode("=", createNode($2), $4);
      }

    | BOOL ID '=' expr ';'
      {
          if(searchSymbol($2) != -1)
          {
              printf("Semantic Error: Variable '%s' already declared.\n", $2);
          }
          else
          {
              insertSymbol($2, TYPE_BOOL);
          }
          printf("Initialized Declaration Found\n");
          $$ = createOperatorNode("=", createNode($2), $4);
      }
    ;

assignment:
      ID '=' expr ';'
      {
          if(searchSymbol($1)==-1)
          {
              printf("Semantic Error: Variable '%s' not declared.\n",$1);
          }
          else
          {
              printf("Assignment Found\n");
          }


          $$ = createOperatorNode("=",
                 createNode($1),
                 $3);
      }
;

if_statement:
      IF '(' expr ')' '{' statements '}'
      {
          printf("If Statement Found\n");
          $$ = createOperatorNode("if", $3, $6);
      }
    ;

while_statement:
      WHILE '(' expr ')' '{' statements '}'
      {
          printf("While Statement Found\n");
          $$ = createOperatorNode("while", $3, $6);
      }
    ;

print_statement:
      PRINT '(' expr ')' ';'
        {
            printf("Print Statement Found\n");
            $$ = createOperatorNode("PRINT", $3, NULL);
        }
    ;
expr:
      expr '+' expr
      {
          $$ = createOperatorNode("+",$1,$3);
      }

    | expr '-' expr
      {
          $$ = createOperatorNode("-",$1,$3);
      }

    | expr '*' expr
      {
          $$ = createOperatorNode("*",$1,$3);
      }

    | expr '/' expr
      {
          $$ = createOperatorNode("/",$1,$3);
      }


    | expr '<' expr
      {
          $$ = createOperatorNode("<",$1,$3);
      }

    | expr '>' expr
      {
          $$ = createOperatorNode(">",$1,$3);
      }

    | expr LE expr
      {
          $$ = createOperatorNode("<=",$1,$3);
      }

    | expr GE expr
      {
          $$ = createOperatorNode(">=",$1,$3);
      }

    | expr EQ expr
      {
          $$ = createOperatorNode("==",$1,$3);
      }

    | expr NE expr
      {
          $$ = createOperatorNode("!=",$1,$3);
      }


    | expr AND expr
      {
          $$ = createOperatorNode("&&",$1,$3);
      }

    | expr OR expr
      {
          $$ = createOperatorNode("||",$1,$3);
      }


    | NOT expr
      {
          $$ = createOperatorNode("!",$2,NULL);
      }


    | '(' expr ')'
      {
          $$=$2;
      }


    | NUMBER
      {
          char buffer[20];
          sprintf(buffer,"%s",yytext);
          $$=createNode(buffer);
      }


    | ID
      {
          if(searchSymbol($1) == -1)
          {
              printf("Semantic Error: Variable '%s' not declared.\n", $1);
          }
          $$=createNode($1);
      }


    | TRUE
      {
          $$=createNode("true");
      }


    | FALSE
      {
          $$=createNode("false");
      }
;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax Error: %s\n", s);
}

int main()
{
    printf("========== COMPILER START ==========\n");

    printf("\n========== LEXICAL ANALYSIS ==========\n");

    initSymbolTable();

    yyparse();

    printf("\n========== SYMBOL TABLE ==========\n");
    printSymbolTable();

    printf("\nParsing Finished.\n");

    return 0;
}
