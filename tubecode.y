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
%token INST_NOP INST_RANDOM INST_OUT_INT INST_OUT_FLOAT INST_OUT_CHAR
%token INST_LOAD INST_STORE INST_MEM_COPY   INST_DEBUG_STATUS
%token ENDLINE 
%token <int_val> ARG_INT ARG_CHAR ARG_REG ARG_IP
%token <lexeme> ARG_LABEL

%type <inst_ptr> statement
%type <arg_ptr> arg_reg arg_const arg_any

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
  | INST_VAL_COPY   arg_any arg_reg         { $$ = new cInst_VAL_COPY(line_num,$2,$3); }
  | INST_ADD        arg_any arg_any arg_reg { $$ = new cInst_ADD(line_num,$2,$3,$4); }
  | INST_SUB        arg_any arg_any arg_reg { $$ = new cInst_SUB(line_num,$2,$3,$4); }
  | INST_MULT       arg_any arg_any arg_reg { $$ = new cInst_MULT(line_num,$2,$3,$4); }
  | INST_DIV        arg_any arg_any arg_reg { $$ = new cInst_DIV(line_num,$2,$3,$4); }
  | INST_MOD        arg_any arg_any arg_reg { $$ = new cInst_MOD(line_num,$2,$3,$4); }
  | INST_TEST_LESS  arg_any arg_any arg_reg { $$ = new cInst_TEST_LESS(line_num,$2,$3,$4); }
  | INST_TEST_GTR   arg_any arg_any arg_reg { $$ = new cInst_TEST_GTR(line_num,$2,$3,$4); }
  | INST_TEST_EQU   arg_any arg_any arg_reg { $$ = new cInst_TEST_EQU(line_num,$2,$3,$4); }
  | INST_TEST_NEQU  arg_any arg_any arg_reg { $$ = new cInst_TEST_NEQU(line_num,$2,$3,$4); }
  | INST_TEST_GTE   arg_any arg_any arg_reg { $$ = new cInst_TEST_GTE(line_num,$2,$3,$4); }
  | INST_TEST_LTE   arg_any arg_any arg_reg { $$ = new cInst_TEST_LTE(line_num,$2,$3,$4); }
  | INST_JUMP       arg_any                 { $$ = new cInst_JUMP(line_num,$2); }
  | INST_JUMP_IF_0  arg_any arg_any         { $$ = new cInst_JUMP_IF_0(line_num,$2,$3); }
  | INST_JUMP_IF_N0 arg_any arg_any         { $$ = new cInst_JUMP_IF_N0(line_num,$2,$3); }
  | INST_NOP                                { $$ = new cInst_NOP(line_num); }
  | INST_RANDOM     arg_any arg_reg         { $$ = new cInst_RANDOM(line_num,$2, $3); }
  | INST_OUT_INT    arg_any                 { $$ = new cInst_OUT_INT(line_num,$2); }
  | INST_OUT_FLOAT  arg_any                 { $$ = new cInst_OUT_FLOAT(line_num,$2); }
  | INST_OUT_CHAR   arg_any                 { $$ = new cInst_OUT_CHAR(line_num,$2); }
  | INST_LOAD       arg_any arg_reg         { $$ = new cInst_LOAD(line_num,$2,$3); }
  | INST_STORE      arg_any arg_any         { $$ = new cInst_STORE(line_num,$2,$3); }
  | INST_MEM_COPY   arg_any arg_any         { $$ = new cInst_MEM_COPY(line_num,$2,$3); }
  | INST_DEBUG_STATUS                       { $$ = new cInst_DEBUG_STATUS(line_num); }
          ;

arg_any:  arg_reg { $$ = $1; }
          | arg_const { $$ = $1; }
          ;

arg_const: ARG_INT { $$ = new cInstArg_Int($1); }
           | ARG_CHAR { $$ = new cInstArg_Int($1); }
           | ARG_LABEL { $$ = new cInstArg_Label($1); }
           ;

arg_reg:  ARG_REG { $$ = new cInstArg_Reg($1); }
          | ARG_IP { $$ = new cInstArg_IP(); }
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
