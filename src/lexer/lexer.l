%{
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "parser.tab.h"

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



"int"           { printf("KEYWORD: int\n"); return INT; }
"float"         { printf("KEYWORD: float\n"); return FLOAT; }
"bool"          { printf("KEYWORD: bool\n"); return BOOL; }

"if"            { printf("KEYWORD: if\n"); return IF; }
"else"          { printf("KEYWORD: else\n"); return ELSE; }
"while"         { printf("KEYWORD: while\n"); return WHILE; }

"print"         { printf("KEYWORD: print\n"); return PRINT; }

"true"          { printf("BOOLEAN: true\n"); return TRUE; }
"false"         { printf("BOOLEAN: false\n"); return FALSE; }



{FLOAT}         { printf("FLOAT: %s\n", yytext); return NUMBER; }
{INT}           { printf("INTEGER: %s\n", yytext); return NUMBER; }



{ID} {
    printf("Identifier : %s\n", yytext);
    yylval.str = strdup(yytext);
    return ID;
    
    
}



"+" { printf("OPERATOR: +\n"); return '+'; }
"-" { printf("OPERATOR: -\n"); return '-'; }
"*" { printf("OPERATOR: *\n"); return '*'; }
"/" { printf("OPERATOR: /\n"); return '/'; }
"=" { printf("OPERATOR: =\n"); return '='; }



"==" { printf("OPERATOR: ==\n"); return EQ; }
"!=" { printf("OPERATOR: !=\n"); return NE; }
"<=" { printf("OPERATOR: <=\n"); return LE; }
">=" { printf("OPERATOR: >=\n"); return GE; }
"&&" { printf("OPERATOR: &&\n"); return AND; }
"||" { printf("OPERATOR: ||\n"); return OR; }
"!"  { printf("OPERATOR: !\n"); return NOT; }
"<"  { printf("OPERATOR: <\n"); return '<'; }
">"  { printf("OPERATOR: >\n"); return '>'; }


";" { printf("PUNCTUATION: ;\n"); return ';'; }
"(" { printf("PUNCTUATION: (\n"); return '('; }
")" { printf("PUNCTUATION: )\n"); return ')'; }
"{" { printf("PUNCTUATION: {\n"); return '{'; }
"}" { printf("PUNCTUATION: }\n"); return '}'; }




[ \t\r]+        { }

\n              { line++; }



. {
    printf("Lexical Error (Line %d): %s\n", line, yytext);
}


%%