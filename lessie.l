%{ 
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include "lessie.tab.h"

int createCnumber();
int truncCreal();

char* createId();

const int MAX_LENGTH = 14; // maximum length of id in characters
%}
%option noyywrap
/* %option c++ */

DIGIT      [0-9]
REAL 	   {DIGIT}+\.{DIGIT}+
WHITESPACE [ \t]
LETTER      [a-zA-Z_]

%%


"if"    { return IF; }
"else"  { return ELSE; }
"return" { return RETURN; }
"for"   { return FOR; }
"struct" {return STRUCT; }
"class" {return CLASS_; }
"def" {return DEF_;}

{WHITESPACE}+ 
"\n" {yylineno++;}


"::" {return COLON_;}
"}"  { return CURLY_CLOSE_; }
"{"  { return CURLY_OPEN_; }
"]"  { return SQUARE_CLOSE_; }
"["  { return SQUARE_OPEN_; }
")"  { return PAR_CLOSE_; }
"("  { return PAR_OPEN_; }
";"  { return SEMICOLON_; }
"+"  { return PLUS; }
"*"  { return MULT; }
"-"  { return MINUS; }
"/"  { return DIV; }
"="  { return ASIGN; }
"==" { return EQUAL; }
"!=" { return NOT_EQUAL; }
">=" { return GREATER_EQUAL; }
"<=" { return LESS_EQUAL; }
"<"  { return LESS; }
">"  { return GREATER; }
"++" { return INC; }
"--" { return DEC; }
"+=" { return ADD_ASIGN; }
"-=" { return MINUS_ASIGN; }
"."  { return POINT_; }
","  { return COMMA; }


L?\"(\\.|[^\\"])*\"	{ return STRING_LITERAL_; }
{REAL}  {yylval.real = truncCreal(); return REAL_;}
{DIGIT}+ { yylval.integer =createCnumber(); return CTI_; }
{LETTER}({LETTER}|{DIGIT})* {  yylval.id = createId(); return ID_; }
"//"[^\n]* 


. { 
  char buffer[200];
  sprintf(buffer, "Unknown character '%s'\n", yytext, yylineno);
  yyerror(buffer);
}

%%


//escapes the first and the last quote and returns the string
char* createStringLiteral()
{
int newLen = yyleng - 2;
char *buffer = (char*)malloc(newLen + 1); 
strncpy(buffer, yytext + 1, newlen);
buffer[newLen] = '\0';
return buffer;
}


/*returns new allocated char* copied from yytext */
char *createId()
{

	if(yyleng >MAX_LENGTH) {
        printf("Warning: Identifier '%s' at line %i will be reduced to %i letters\n", yytext, yylineno, MAX_LENGTH); 
		yyleng = MAX_LENGTH;
    }
	char *buffer = (char*)malloc(yyleng + 1); 
	strncpy(buffer, yytext, yyleng);
    buffer[yyleng] = '\0';
	return buffer;
}

double truncCreal()
{
  return atof(yytext);
}

int createCnumber()
{
  return atoi(yytext); 
}
  
/*MAIN*/
int main(int argc, char **argv) {
  char *fileName;
  int i, n = 0;
  for(i = 0; i<argc; ++i) {
    if(strcmp(argv[i], "-v") == 0) { verbose = TRUE; n++; }
    else if(strcmp(argv[i], "-t") == 0) { showTDS = TRUE; n++; }
  }
  --argc; 
  n++;
  
  if (argc == n) {
    if((yyin = fopen (argv[argc], "r")) == NULL) 
        fprintf(stderr, "Invalid file %s\n", argv[argc]);
    else {
        if (verbose == TRUE) fprintf(stdout, "$3d.- ", yylineno);
        fileName = argv[argc];
        yyparse();
        if (numErrores == 0) ;
        else fprintf(stdout, "\nNumber of errors: %d\n", numErrores); 
     }
   }
   else fprintf(stderr, "Usage: lessie [-v] [-t] file\n");

  return 0;
}


