%{
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "inst.h"
#include "hardware.h"

extern int line_num;
extern int yylex();
extern cHardware * main_hardware;

void yyerror(std::string err_string) {
  // std::cout << "ERROR(line " << line_num << "): " << err_string << std::endl;
  (*main_hardware) << "ERROR(line " << line_num << "): " << err_string << '\n';
}

%}

%union {
  int int_val;
  float float_val;
  char * lexeme;
  cInst_Base * inst_ptr;
  cInstArg_Base * arg_ptr;
}

%token INST_VAL_COPY INST_ADD INST_SUB INST_MULT INST_DIV INST_MOD
%token INST_TEST_LESS INST_TEST_GTR INST_TEST_EQU INST_TEST_NEQU INST_TEST_GTE INST_TEST_LTE
%token INST_JUMP INST_JUMP_IF_0 INST_JUMP_IF_N0
%token INST_NOP INST_RANDOM INST_OUT_INT INST_OUT_FLOAT INST_OUT_CHAR INST_PUSH INST_POP

%token INST_AR_GET_IDX INST_AR_SET_IDX INST_AR_GET_SIZ INST_AR_SET_SIZ INST_AR_COPY
%token INST_AR_PUSH INST_AR_POP
%token ENDLINE 
%token <int_val> ARG_INT ARG_SCALAR ARG_CHAR ARG_ARRAY
%token <float_val> ARG_FLOAT
%token <lexeme> ARG_LABEL

%type <inst_ptr> statement
%type <arg_ptr> arg_var arg_arr arg_const arg_any

%%

program:      statement_list { ; }
        ;

statement_list:	{ ; }
	|	statement_list statement ENDLINE {
		  if ($2 != NULL) main_hardware->AddInst($2);
		}
	|	statement_list ARG_LABEL ':' statement ENDLINE {
                  main_hardware->AddLabel($2);
		  if ($4 != NULL) main_hardware->AddInst($4);
		}
	;

statement:   { $$ = NULL; }
  | INST_VAL_COPY   arg_any arg_var         { $$ = new cInst_VAL_COPY(line_num,$2,$3); }
  | INST_ADD        arg_any arg_any arg_var { $$ = new cInst_ADD(line_num,$2,$3,$4); }
  | INST_SUB        arg_any arg_any arg_var { $$ = new cInst_SUB(line_num,$2,$3,$4); }
  | INST_MULT       arg_any arg_any arg_var { $$ = new cInst_MULT(line_num,$2,$3,$4); }
  | INST_DIV        arg_any arg_any arg_var { $$ = new cInst_DIV(line_num,$2,$3,$4); }
  | INST_MOD        arg_any arg_any arg_var { $$ = new cInst_MOD(line_num,$2,$3,$4); }
  | INST_TEST_LESS  arg_any arg_any arg_var { $$ = new cInst_TEST_LESS(line_num,$2,$3,$4); }
  | INST_TEST_GTR   arg_any arg_any arg_var { $$ = new cInst_TEST_GTR(line_num,$2,$3,$4); }
  | INST_TEST_EQU   arg_any arg_any arg_var { $$ = new cInst_TEST_EQU(line_num,$2,$3,$4); }
  | INST_TEST_NEQU  arg_any arg_any arg_var { $$ = new cInst_TEST_NEQU(line_num,$2,$3,$4); }
  | INST_TEST_GTE   arg_any arg_any arg_var { $$ = new cInst_TEST_GTE(line_num,$2,$3,$4); }
  | INST_TEST_LTE   arg_any arg_any arg_var { $$ = new cInst_TEST_LTE(line_num,$2,$3,$4); }
  | INST_JUMP       arg_any                 { $$ = new cInst_JUMP(line_num,$2); }
  | INST_JUMP_IF_0  arg_any arg_any         { $$ = new cInst_JUMP_IF_0(line_num,$2,$3); }
  | INST_JUMP_IF_N0 arg_any arg_any         { $$ = new cInst_JUMP_IF_N0(line_num,$2,$3); }
  | INST_NOP                                { $$ = new cInst_NOP(line_num); }
  | INST_RANDOM     arg_any arg_var         { $$ = new cInst_RANDOM(line_num,$2,$3); }
  | INST_OUT_INT    arg_any                 { $$ = new cInst_OUT_INT(line_num,$2); }
  | INST_OUT_FLOAT  arg_any                 { $$ = new cInst_OUT_FLOAT(line_num,$2); }
  | INST_OUT_CHAR   arg_any                 { $$ = new cInst_OUT_CHAR(line_num,$2); }
  | INST_PUSH       arg_any                 { $$ = new cInst_PUSH_NUM(line_num,$2); }
  | INST_POP        arg_var                 { $$ = new cInst_POP_NUM(line_num,$2); }
  | INST_AR_GET_IDX arg_arr arg_any arg_var { $$ = new cInst_AR_GET_IDX(line_num,$2,$3,$4); }
  | INST_AR_SET_IDX arg_arr arg_any arg_any { $$ = new cInst_AR_SET_IDX(line_num,$2,$3,$4); }
  | INST_AR_GET_SIZ arg_arr arg_var         { $$ = new cInst_AR_GET_SIZ(line_num,$2,$3); }
  | INST_AR_SET_SIZ arg_arr arg_any         { $$ = new cInst_AR_SET_SIZ(line_num,$2,$3); }
  | INST_AR_COPY    arg_arr arg_arr         { $$ = new cInst_AR_COPY(line_num,$2,$3); }
  | INST_AR_PUSH    arg_arr                 { $$ = new cInst_PUSH_ARRAY(line_num,$2); }
  | INST_AR_POP     arg_arr                 { $$ = new cInst_POP_ARRAY(line_num,$2); }
  | ARG_LABEL {
       std::string err = "Unknown instruction '";
       err += $1;
       err += "'.";
       yyerror(err);
       exit(1);
    }
          ;

arg_any:  arg_var { $$ = $1; }
          | arg_const { $$ = $1; }
          ;

arg_const: ARG_FLOAT { $$ = new cInstArg_Float($1); }
           | ARG_CHAR { $$ = new cInstArg_Float($1); }
           | ARG_LABEL { $$ = new cInstArg_Label($1); }
           ;

arg_var:  ARG_SCALAR { $$ = new cInstArg_Var($1); }
          ;

arg_arr:  ARG_ARRAY { $$ = new cInstArg_Array($1); }
          ;

%%
void LexMain(int argc, char * argv[]);
void LexReadString(const std::string & in_string);

int main(int argc, char * argv[])
{
  main_hardware = new cHardware();
  LexMain(argc, argv);
  yyparse();

  main_hardware->Run();

  return 0;
}

bool ParseString(const std::string & in_string)
{
  LexReadString(in_string);
  yyparse();
  // yy_delete_buffer(YY_CURRENT_BUFFER);

  return true;
}
