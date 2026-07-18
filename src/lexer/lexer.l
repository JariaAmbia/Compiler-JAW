%{
#include <stdio.h>
#include <stdlib.h>

#include "../parser/parser.tab.h"

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



"int"           { return INT; }
"float"         { return FLOAT; }
"bool"          { return BOOL; }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }

"print"         { return PRINT; }

"true"          { return TRUE; }
"false"         { return FALSE; }




{FLOAT}         { return NUMBER; }
{INT}           { return NUMBER; }



{ID}            { return ID; }



"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }



"="             { return '='; }

"=="            { return EQ; }
"!="            { return NE; }
"<="            { return LE; }
">="            { return GE; }

"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

"<"             { return '<'; }
">"             { return '>'; }


";"             { return ';'; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }




[ \t\r]+        { }

\n              { line++; }



. {
    printf("Lexical Error (Line %d): %s\n", line, yytext);
}


%%
