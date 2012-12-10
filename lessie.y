%{
#include <stdio.h>
#include "common.h"

#include "virtual_machine/instruction.h"
#include "virtual_machine/classgenerator.h"

extern int yylineno;
int verbose = FALSE;
int showTDS = FALSE;
int numErrores = 0;
int level = 0;

ClassGenerator generator;

%}
%union {
    char *string_literal;
    char *id;
    double real;
    int integer;
    int hasRef;
    struct {
        int label;
        int ref;
    } forHelper;
   
    int incrementOperator;
    int multiplicativeOperator;
    int additiveOperator;
   // int asignationOperator;
    
    int relationalOperator;
    int equalityOperator;
}

%error-verbose
%token <real> REAL_
%token <id>ID_
%token <integer>CTI_
%token <string_literal> STRING_LITERAL_
%token CLASS_
%token DEF_
%token COLON_

%token NEW_
%token VAR_



%token SEMICOLON_

%token SQUARE_OPEN_ SQUARE_CLOSE_
%token CURLY_OPEN_ CURLY_CLOSE_
%token ASSIGN
%token EQUAL NOT_EQUAL GREATER LESS GREATER_EQUAL LESS_EQUAL
%token PLUS MINUS
%token MULT DIV_
%token FOR
%token INC_ DEC_
%token IF ELSE
%token RETURN
%right POINT_
%token COMMA
%left PAR_OPEN_ PAR_CLOSE_

%type<integer> formalParameterList;
%type<integer> formalParameters;
%type<integer> actualParameterList;
%type<integer> actualParameters;
%type <integer> incrementOperator;
%type <integer> multiplicativeOperator;
%type <integer> additiveOperator;
%type <integer> unaryOperator;
%type <integer> relationalOperator;
%type <integer> equalityOperator;


%% 
program : CLASS_ ID_ 
	{
	  generator.setClassName($2);
	
	}CURLY_OPEN_ declarationList CURLY_CLOSE_
	{
	generator.finalizeClass();	
	};
declarationList : declaration | declarationList declaration;
declaration : memberDeclaration | functionDeclaration;
memberDeclaration : VAR_ ID_ SEMICOLON_{
		  generator.addField($2);		  
		  } ; 



/* type : INT_ | STRING_ | STRUCT ID_; */
/* structDeclaration : STRUCT ID_ CURLY_OPEN_ structBody CURLY_CLOSE_; */
/*structBody : structBody memberDeclaration | memberDeclaration;*/
/*memberDeclaration : variableDeclaration | functionDeclaration;*/

functionDeclaration : functionHead block
		    {
		      generator.finalizeMethod();
		    
		    };
functionHead : DEF_ ID_ 
	     {
	       generator.createMethod($2,0);
	     
	     }
	     PAR_OPEN_ formalParameters PAR_CLOSE_
	     {
	       generator.setParamsCountForCurrentMethod($5);
	     
	     };
formalParameters : {$$ = 0;} /* eps */ | formalParameterList {$$ = $1};

		    /* PARAMETERS DECLARATION */
formalParameterList : ID_ 
		    {
		    	generator.allocLocal($1);
			$$ = 1;
		    }
		    | ID_ COMMA
		    {
		    	generator.allocLocal($1);
		    }
		    formalParameterList 
		    {
		    		    
			$$ = $4 + 1;
		    }		  
		    ;

block : CURLY_OPEN_ localVariableDeclaration instructionList CURLY_CLOSE_ ; 
localVariableDeclaration : /* eps */ | localVariableDeclaration variableDeclaration;
		     /* LOCAL VARIABLES DECLARATION */			 
variableDeclaration : VAR_ ID_ SEMICOLON_ 
		    {
		    	generator.allocLocal($2);		    
		    }
		    | VAR_  ID_ SQUARE_OPEN_ CTI_ SQUARE_CLOSE_  SEMICOLON_ 
		    {
		    	generator.allocLocal($2);
		    	generator.emitNewArray($4);
			generator.storeLocal($2);
		    }| VAR_ ID_ ASSIGN expression SEMICOLON_
		    {
		    	//expression leaves a value to store on the stack
		    	generator.allocLocal($2);			
			generator.storeLocal($2);		    
		    }
		    ; 
instructionList : /* eps */  | instructionList instruction ;
instruction : CURLY_OPEN_ localVariableDeclaration instructionList CURLY_CLOSE_ |
                expressionInstruction |assignExpression SEMICOLON_ | selectionInstruction | iterationInstruction | returnInstruction;
expressionInstruction : SEMICOLON_ | expression SEMICOLON_;

selectionInstruction : IF PAR_OPEN_ expression PAR_CLOSE_
		     {
		       /*emit jump for false eval ie if expression <= 0*/
		       generator.emitPush(0);
		       generator.emit(SUB);
		       $<hasRef>$ = generator.emitNonCompleteJump(IF_LE);//emit conditional jump to the else part
		     
		     }instruction 
		     {
		     //jump to after else
		      $<hasRef>$ = generator.emitNonCompleteJump(JMP);
		      //finalize jump to the else part
		      generator.completeAddress($<hasRef>5);
		     }
		     ELSE instruction 
		     {
		     //finalize jump - completeaddress
		     generator.completeAddress($<hasRef>7);
		     };
iterationInstruction : FOR PAR_OPEN_ optionalExpression SEMICOLON_ 
		     {
		        // Start:
                        $<forHelper>$.label = generator.nextInsAddress;
		     }
		     expression 
		     {
		      generator.emitPush(0);
		      generator.emit(SUB);
		      //create jmp instruction from here to final - in case of false expression evaluation
		      $<forHelper>$.ref = generator.emitNonCompleteJump(IF_LE);

		     }
		     SEMICOLON_ 
		       {
		         //jump to body
		         $<forHelper>$.ref = generator.emitNonCompleteJump(JMP);
			 //optExpression:remember addres for optional expression - increment expression
                         $<forHelper>$.label = generator.nextInsAddress;
                        }
		     optionalExpression
		     {    
		       //jump back to start end evaluate condition
		       generator.emitSimpleJump(JMP, $<forHelper>5.label - generator.nextInsAddress - 2);
                       // complete jump from expression to body:
                       generator.completeAddress($<forHelper>9.ref);
                        }
		     PAR_CLOSE_  instruction
		     {
		      // jump to optExpression
		      generator.emitSimpleJump(JMP, $<forHelper>9.label - generator.nextInsAddress - 2);
                      // complete jump for false expression evaluation - ie end of for cycle
                      generator.completeAddress($<forHelper>7.ref);
		     }
		     ;
optionalExpression : /* eps */ | assignExpression;
returnInstruction :  
RETURN expression SEMICOLON_ 
		  {
		  generator.emit(RET);
		  }
		  | RETURN SEMICOLON_
		  {
		  generator.emit(RET_VOID);		  
		  };




assignExpression: ID_ ASSIGN expression 
		{
		  generator.storeLocal($1); 		    	
		}
		|ID_ POINT_ ID_ ASSIGN expression 
		{
		  generator.storeMember($1,$3);
		}
		| ID_ SQUARE_OPEN_ expression SQUARE_CLOSE_ ASSIGN expression 
		{
		  //TODO: CHANGE ORDER OF PARAMETERS FOR STORE ARRAY IN VM: [index, value, arrayRef] <- TOP
		  generator.loadLocal($1);
                  generator.emit(STORE_ARRAY);
		}
		;

expression : equalityExpression ;

equalityExpression : relationalExpression | equalityExpression equalityOperator relationalExpression 
		    {
		     generator.emit(SUB);
		     generator.emitSimpleJump((INSTRUCTION)$2,5);
		     generator.emitPush(0);
		     generator.emitSimpleJump(JMP,3);
		     generator.emitPush(1);		     
		     };
relationalExpression : additiveExpression | relationalExpression relationalOperator additiveExpression 
		     {
		     generator.emit(SUB);
		     generator.emitSimpleJump((INSTRUCTION)$2,5);
		     generator.emitPush(0);
		     generator.emitSimpleJump(JMP,3);
		     generator.emitPush(1);		     
		     }
		     ;
additiveExpression : multiplicativeExpression | additiveExpression additiveOperator multiplicativeExpression{generator.emit((INSTRUCTION)$2);};
multiplicativeExpression : unaryExpression | multiplicativeExpression multiplicativeOperator unaryExpression{generator.emit((INSTRUCTION)$2);};
			 
unaryExpression : suffixExpression | 
		unaryOperator unaryExpression
		{
		  if($1 == -1)
		  	{
			  generator.emitPush(-1);
			  generator.emit(MUL);			
			}	
		}
		;

suffixExpression :  
		 memberAccess incrementOperator 
		 {
		   //assuming that on the stack si value to increment
		  generator.emit((INSTRUCTION)$2);
		 }
     |PAR_OPEN_ expression PAR_CLOSE_ 
     |NEW_ ID_
     {generator.emitNew($2); generator.emit(DUP);}
     PAR_OPEN_ actualParameters PAR_CLOSE_
     {     	
            
	generator.emitCall(CALL_DYNAMIC, $2, $5);
     }
     | functionCall | memberAccess |
      ID_ SQUARE_OPEN_ expression SQUARE_CLOSE_ 
	    {
	     generator.loadLocal($1);
	     generator.emit(LOAD_ARRAY);
	    
	    }
    | CTI_ 
    {
      generator.emitPush($1);
    }
    |STRING_LITERAL_
    {
       generator.emitPush($1);	    
    }
    |REAL_
    {
    generator.emitPush($1);	    
    }
     
    ;

functionCall: ID_  POINT_ ID_ PAR_OPEN_ actualParameters PAR_CLOSE_ 
	    {
	    	//push self as $1 object
		generator.loadLocal($1);
	    	//stores the reference of the object on to stack, ie PUSH(objectRef), object refenrce is retrieved by neame from $1 ;
	    	generator.loadLocal($1);
		//emits a member method call determined by its name in $3, $5(actualParameters) contains the count of the parameters
		generator.emitCall(CALL_DYNAMIC, $3, $5);
	    }|
	    /*CALL OF STATIC FUNCTION */
	    ID_ COLON_ ID_ PAR_OPEN_ actualParameters PAR_CLOSE_ 
	    {
		//static call $3- method name, $1 - class name, $5 - parameters count
		generator.emitCall(CALL, $3, $5, $1);	    
	    }|
	    ID_ PAR_OPEN_ actualParameters PAR_CLOSE_ 
	    /*CALL local method - same as calling self.method(params)*/
	    {
	    	//push self as the first parameter of the function to call
		generator.loadLocal("self");
	    	//stores the reference of the object on to stack, ie PUSH(objectRef), this parameter is for CALL_DYNAMIC instruction ;	
	    	generator.loadLocal("self");
		//emits a member method call determined by its name in $3, $5(actualParameters) contains the count of the parameters
		generator.emitCall(CALL_DYNAMIC, $1, $3);	    
	    }
	    ;

memberAccess:	   
	    ID_ POINT_ ID_
	    {
	    	//pushes the object member variable onto the stack, determined by the method name - $1(ID_) and member name $3(ID_)
		generator.loadMember($1,$3);	    
	    }|ID_
	    {
	    //load local variable(preferably) or member variable on the stack
	    if(generator.localExist($1))
	    	generator.loadLocal($1);
		else
		generator.loadMember("self",$1);
	    };

actualParameters : /* eps */ 
		 {
		 //empty parameter list - returns 0
		 $$ = 0;
		 }
		 |
		 actualParameterList
		 {
		 //count of parameters is returned from $1(actualParameterList)
		 $$ = $1;
		 
		 };
actualParameterList : expression
		    {
		    //returns 1 as parameters count
		    $$ = 1;
		    }
		    | expression COMMA actualParameterList
		    {
		    //returns parameters count parsed so far from $3(actualParameterList) plus 1
		    $$ = $3+1;
		    };
/*asignationOperator : ASIGN | ADD_ASIGN | MINUS_ASIGN ;*/
equalityOperator : EQUAL{$$=IF_EQ;} | NOT_EQUAL{$$=IF_NE;};
relationalOperator : GREATER {$$=IF_GT;} | LESS {$$=IF_LT;}| GREATER_EQUAL {$$=IF_GE;}| LESS_EQUAL {$$=IF_LE;};
additiveOperator : PLUS {$$ = ADD;}| MINUS{$$ = SUB;};
multiplicativeOperator : MULT {$$ = MUL;} | DIV_ {$$ = DIV;};
incrementOperator : INC_ {$$ = INC;} | DEC_ {$$ = DEC;};
unaryOperator : PLUS {$$ = 1;}| MINUS {$$ = -1;};





%%

int yyerror(const char *s) 
{
  printf("Line %d: %s\n", yylineno, s);
}

