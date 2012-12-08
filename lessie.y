%{
#include <stdio.h>
#include "common.h"
extern int yylineno;
int verbose = FALSE;
int showTDS = FALSE;
int numErrores = 0;
int level = 0;

%}
%union {
    char *string_literal;
    char *id;
    int integer;
    struct {
        int type; // STRUCT or INTEGER
        int ref; // n.ref if it is a struct
        int size; 
    } type;
    struct {
        int type;
        char *name;
        int size; // In case of arrays this is not equal to type.size
        int ref;
    } variableDeclaration;
    struct {
        int returnType; // STRUCT or INTEGER
        int returnTypeRef;
        int desp;
        int parameterRef; 
        char *name;
    } functionHead;
    struct {
        int desp;
        int parameterRef; // "Dominio"
    } formalParameters, formalParameterList;
    struct {
        int fieldsRef;
        int desp;
    } fieldList;
    struct {
        int desp;
    } declarationList;
    struct {
        int desp;
    } localVariableDeclaration;
    struct {
        int pos;
        int type; 
        int size;
    } expression;
    struct {
        int oldDvar;
    } block; 
    struct {
        int label;
        int ref;
    } forHelper;
    struct {
        int ref;
    } actualParameters, actualParameterList;
    int incrementOperator;
    int multiplicativeOperator;
    int additiveOperator;
    int asignationOperator;
    int hasRef;
    int relationalOperator;
    int equalityOperator;
}

%error-verbose
%token <real> REAL_
%token <id>ID_
%token <integer>CTI_
%token <string_literal> STRING_LITERAL_
%token INT_ 
%token PRINT_ READ_
%token ASIG_ MAS_ POR_
%token SEMICOLON_
%token PAR_OPEN_ PAR_CLOSE_
%token SQUARE_OPEN_ SQUARE_CLOSE_
%token CURLY_OPEN_ CURLY_CLOSE_
%token ASIGN ADD_ASIGN MINUS_ASIGN
%token EQUAL NOT_EQUAL GREATER LESS GREATER_EQUAL LESS_EQUAL
%token PLUS MINUS
%token MULT DIV
%token FOR
%token INC DEC
%token IF ELSE
%token RETURN STRUCT
%token POINT_
%token COMMA
%token STRING_ STRING_LITERAL_
%%
program : CLASS_ CURLY_OPEN_ declarationList CURLY_CLOSE_;
declarationList : declaration | declarationList declaration;
declaration : variableDeclaration | functionDeclaration;
variableDeclaration : VAR_ ID_ SEMICOLON_ | VAR_  ID_ SQUARE_OPEN_ CTI_ SQUARE_CLOSE_  SEMICOLON_ ; 

/* type : INT_ | STRING_ | STRUCT ID_; */
/* structDeclaration : STRUCT ID_ CURLY_OPEN_ structBody CURLY_CLOSE_; */
/*structBody : structBody memberDeclaration | memberDeclaration;*/
/*memberDeclaration : variableDeclaration | functionDeclaration;*/

functionDeclaration : functionHead block;
functionHead : DEF_ ID_ PAR_OPEN_ formalParameters PAR_CLOSE_;
formalParameters : /* eps */ | formalParameterList ;
formalParameterList : ID_ | ID_ COMMA formalParameterList ;
block : CURLY_OPEN_ localVariableDeclaration instructionList CURLY_CLOSE_ ; 
localVariableDeclaration : /* eps */ | localVariableDeclaration variableDeclaration;
instructionList : /* eps */  | instructionList instruction ;
instruction : CURLY_OPEN_ localVariableDeclaration instructionList CURLY_CLOSE_ |
                expressionInstruction | ioInstruction | selectionInstruction | iterationInstruction | returnInstruction;
expressionInstruction : SEMICOLON_ | expression SEMICOLON_;

selectionInstruction : IF PAR_OPEN_ expression PAR_CLOSE_  instruction ELSE instruction ;
iterationInstruction : FOR PAR_OPEN_ optionalExpression SEMICOLON_ expression SEMICOLON_ optionalExpression PAR_CLOSE_  instruction;
optionalExpression : /* eps */ | expression;
returnInstruction : RETURN expression SEMICOLON_ ;
expression : equalityExpression | ID_ asignationOperator expression | ID_ SQUARE_OPEN_ expression SQUARE_CLOSE_ asignationOperator expression | 
             ID_ POINT_ ID_ asignationOperator expression | STRING_LITERAL_;
equalityExpression : relationalExpression | equalityExpression equalityOperator relationalExpression ;
relationalExpression : additiveExpression | relationalExpression relationalOperator additiveExpression ;
additiveExpression : multiplicativeExpression | additiveExpression additiveOperator multiplicativeExpression;
multiplicativeExpression : unaryExpression | multiplicativeExpression multiplicativeOperator unaryExpression;
unaryExpression : suffixExpression | unaryOperator unaryExpression | incrementOperator ID_;
suffixExpression : ID_ SQUARE_OPEN_ expression SQUARE_CLOSE_ | memberAccess | ID_ incrementOperator | 
    ID_ PAR_OPEN_ actualParameters PAR_CLOSE_ | PAR_OPEN_ expression PAR_CLOSE_ | ID_ | CTI_;
memberAccess: ID_ POINT_ ID_ PAR_OPEN_ actualParameters PAR_CLOSE_ | ID_ POINT_ ID_;
actualParameters : /* eps */ | actualParameterList;
actualParameterList : expression | expression COMMA actualParameterList;
asignationOperator : ASIGN | ADD_ASIGN | MINUS_ASIGN ;
equalityOperator : EQUAL | NOT_EQUAL;
relationalOperator : GREATER | LESS | GREATER_EQUAL | LESS_EQUAL ;
additiveOperator : PLUS | MINUS;
multiplicativeOperator : MULT | DIV;
incrementOperator : INC | DEC;
unaryOperator : PLUS | MINUS;





%%

int yyerror(const char *s) 
{
  printf("Line %d: %s\n", yylineno, s);
}

