%{

#include "inst.h"
#include "hardware.h"
#include "tubecode.tab.hh"

#include <iostream>
#include <cstdlib>
#include <stdio.h>

int line_num = 1;
cHardware * main_hardware;
%}

%option nounput
%option noyywrap

int		[0-9]+
float           [0-9]*\.?[0-9]+
comment		#.*
eol		\n
whitespace	[ \t\r]

%%

val_copy { return INST_VAL_COPY; }
add { return INST_ADD; }
sub { return INST_SUB; }
mult { return INST_MULT; }
div { return INST_DIV; }
mod { return INST_MOD; }
test_less { return INST_TEST_LESS; }
test_gtr { return INST_TEST_GTR; }
test_equ { return INST_TEST_EQU; }
test_nequ { return INST_TEST_NEQU; }
test_gte { return INST_TEST_GTE; }
test_lte { return INST_TEST_LTE; }
ju?mp { return INST_JUMP; }
ju?mp_if_0 { return INST_JUMP_IF_0; }
ju?mp_if_n(ot)?0 { return INST_JUMP_IF_N0; }
nop { return INST_NOP; }
random { return INST_RANDOM; }
out_int { return INST_OUT_INT; }
out_float|out_val { return INST_OUT_FLOAT; }
out_char { return INST_OUT_CHAR; }

load { return INST_LOAD; }
store { return INST_STORE; }
mem_copy { return INST_MEM_COPY; }

debug_status { return INST_DEBUG_STATUS; }

(push)|(pop)|(ar(ray)?_get_(idx|index))|(ar(ray)?_set_(idx|index))|(ar(ray)?_get_siz(e?))|(ar(ray)?_set_siz(e?))|(ar(ray)?_copy)|(ar(ray)?_push)|(ar(ray)?_pop)|((a|s){int}) { std::cerr << "Error(line " << line_num << "): instruction '" << yytext << "' valid only in TubeIC, not TubeCode assembly." << std::endl; exit(1); }

-?{float} { yylval.float_val = atof(yytext); return ARG_FLOAT; }
reg[A-H] { yylval.int_val = yytext[3]-'A'; return ARG_REG; }
reg[I-Z] { std::cerr << "Error(line " << line_num << "): " << yytext << "not a legal register; only 8 registers available." << std::endl; exit(1); }
IP { return ARG_IP; }
'.' { yylval.int_val = (int) yytext[1]; return ARG_CHAR; }
'\\n' { yylval.int_val = (int) '\n'; return ARG_CHAR; }
'\\t' { yylval.int_val = (int) '\t'; return ARG_CHAR; }
'\\'' { yylval.int_val = (int) '\''; return ARG_CHAR; }
'\\\\' { yylval.int_val = (int) '\\'; return ARG_CHAR; }
'\\\"' { yylval.int_val = (int) '\"'; return ARG_CHAR; }
[a-zA-Z][a-zA-Z0-9_]* { yylval.lexeme = strdup(yytext); return ARG_LABEL; }

[:] { return yytext[0]; }

{eol}  { line_num++; return ENDLINE; }
{comment} { ; }
{whitespace} { ; }
.      { std::cout << "ERROR(line " << line_num << "): Unknown Token '" << yytext << "'." << std::endl; exit(1); }

%%

void LexMain(int argc, char * argv[])
{
  int arg_id = 0;
  while (true) {
    arg_id++;
    if (arg_id >= argc) {
      std::cerr << "Format: " << argv[0] << "[flags] [filename]" << std::endl;
      std::cerr << "Type '" << argv[0] << " -h' for help." << std::endl;
      exit(1);
    }

    std::string cur_arg(argv[arg_id]);

    if (cur_arg == "-c") {
      main_hardware->CountCPUCycles();
      continue;
    }

    if (cur_arg == "-h") {
      std::cout << "Tube Code Assembly v. 0.1"  << std::endl
           << "Format: " << argv[0] << "[flags] [filename]" << std::endl
           << std::endl
           << "Flags:" << std::endl
           << "  -c  :  Count CPU cycles" << std::endl
           << "  -h  :  Help (this information)" << std::endl
           << "  -i  :  List Instructions" << std::endl
           << "  -t  [timeout] :  Set a max number of instructions executed before halting" << std::endl
           << "  -v  :  Verbose.  Print information about each line executed to trace.dat" << std::endl
        ;
      exit(0);
    }

    if (cur_arg == "-i") {
      std::cout << "Available Instructions:" << std::endl;
      std::cout << "  " << cInst_VAL_COPY::GetDesc() << std::endl;
      std::cout << "  " << cInst_ADD::GetDesc() << std::endl;
      std::cout << "  " << cInst_SUB::GetDesc() << std::endl;
      std::cout << "  " << cInst_MULT::GetDesc() << std::endl;
      std::cout << "  " << cInst_DIV::GetDesc() << std::endl;
      std::cout << "  " << cInst_MOD::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_LESS::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_GTR::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_EQU::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_NEQU::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_GTE::GetDesc() << std::endl;
      std::cout << "  " << cInst_TEST_LTE::GetDesc() << std::endl;
      std::cout << "  " << cInst_JUMP::GetDesc() << std::endl;
      std::cout << "  " << cInst_JUMP_IF_0::GetDesc() << std::endl;
      std::cout << "  " << cInst_JUMP_IF_N0::GetDesc() << std::endl;
      std::cout << "  " << cInst_NOP::GetDesc() << std::endl;
      std::cout << "  " << cInst_RANDOM::GetDesc() << std::endl;
      std::cout << "  " << cInst_OUT_INT::GetDesc() << std::endl;
      std::cout << "  " << cInst_OUT_FLOAT::GetDesc() << std::endl;
      std::cout << "  " << cInst_OUT_CHAR::GetDesc() << std::endl;
      std::cout << "  " << cInst_LOAD::GetDesc() << std::endl;
      std::cout << "  " << cInst_STORE::GetDesc() << std::endl;
      std::cout << "  " << cInst_MEM_COPY::GetDesc() << std::endl;
      std::cout << "  " << cInst_DEBUG_STATUS::GetDesc() << std::endl;
      exit(0);
    }

    if (cur_arg == "-t") {
      int timeout;
      arg_id++;
      std::stringstream(argv[arg_id]) >> timeout;
      main_hardware->SetTimeout(timeout);
      continue;
    }

    if (cur_arg == "-v") {
      main_hardware->SetVerbose();
      continue;
    }

    // The only thing left to do is assume the current argument is the filename.
    FILE *file = fopen(argv[arg_id], "r");
    if (!file) {
      std::cerr << "Error opening " << cur_arg << std::endl;
      exit(2);
    }
    yyin = file;
    return;
  }

  return;
}

void LexReadString(const std::string & in_string)
{
  yy_scan_string(in_string.c_str());  
}
