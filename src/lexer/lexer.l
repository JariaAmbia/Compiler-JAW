%{
#include <stdio.h>
#include <stdlib.h>

int line = 1;
extern FILE *yyin;
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
INT         {DIGIT}+
FLOAT       {DIGIT}+"."{DIGIT}+

%%





"int"           { printf("KEYWORD      : int\n"); }
"float"         { printf("KEYWORD      : float\n"); }
"bool"          { printf("KEYWORD      : bool\n"); }

"if"            { printf("KEYWORD      : if\n"); }
"else"          { printf("KEYWORD      : else\n"); }
"while"         { printf("KEYWORD      : while\n"); }

"print"         { printf("KEYWORD      : print\n"); }

"true"          { printf("BOOLEAN      : true\n"); }
"false"         { printf("BOOLEAN      : false\n"); }





{FLOAT}         { printf("FLOAT        : %s\n",yytext); }

{INT}           { printf("INTEGER      : %s\n",yytext); }





{ID}            { printf("IDENTIFIER   : %s\n",yytext); }





"+"             { printf("PLUS         : +\n"); }

"-"             { printf("MINUS        : -\n"); }

"*"             { printf("MULTIPLY     : *\n"); }

"/"             { printf("DIVIDE       : /\n"); }

"%"             { printf("MODULUS      : %%\n"); }





"="             { printf("ASSIGN       : =\n"); }





"=="            { printf("EQUAL        : ==\n"); }

"!="            { printf("NOT EQUAL    : !=\n"); }

"<="            { printf("LESS EQUAL   : <=\n"); }

">="            { printf("GREATER EQ   : >=\n"); }

"<"             { printf("LESS THAN    : <\n"); }

">"             { printf("GREATER THAN : >\n"); }




"&&"            { printf("AND          : &&\n"); }

"||"            { printf("OR           : ||\n"); }

"!"             { printf("NOT          : !\n"); }




";"             { printf("SEMICOLON    : ;\n"); }

","             { printf("COMMA        : ,\n"); }

"("             { printf("LPAREN       : (\n"); }

")"             { printf("RPAREN       : )\n"); }

"{"             { printf("LBRACE       : {\n"); }

"}"             { printf("RBRACE       : }\n"); }



"//".*          { }

"/*"([^*]|\*+[^*/])*\*+"/"    { }




[ \t\r]+        { }

\n              { line++; }





.               {
                    printf("LEXICAL ERROR (Line %d): %s\n",
                    line,yytext);
                }

%%
int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        printf("Cannot open file: %s\n", argv[1]);
        return 1;
    }

    yyin = fp;

    printf("\n========== LEXICAL ANALYSIS ==========\n\n");

    yylex();

    printf("\n======================================\n");
    printf("Lexical Analysis Completed Successfully.\n");

    fclose(fp);

    return 0;
}