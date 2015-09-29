#ifndef INST_H
#define INST_H

#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class cHardware;

class cInstArg_Base {
protected:
  cHardware * hardware;
public:
  cInstArg_Base() { ; }
  virtual ~cInstArg_Base() { ; }

  virtual bool IsVar() { return false; }
  virtual bool SetFloat(float value) = 0 ;//{ assert(false); (void) value; return false; }

  virtual int AsInt() = 0;
  virtual float AsFloat() = 0;
  virtual std::string VerboseString() = 0;

  void SetHardware(cHardware * _h) { hardware = _h; }
};

class cInstArg_Float : public cInstArg_Base {
private:
  float value;
public:
  cInstArg_Float(float _v) : value(_v) { ; }
  ~cInstArg_Float() { ; }

  bool SetFloat(float value) {
    assert(false && "Calling set on cInstArg_Float");
    (void) value;
    return false;
  }

  int AsInt() { return value; }
  float AsFloat() { return value; }
  std::string VerboseString() {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }
};

class cInstArg_Label : public cInstArg_Base {
private:
  std::string label;
  int value;
public:
  cInstArg_Label(std::string _l) : label(_l), value(-1) { ; }
  ~cInstArg_Label() { ; }

  bool SetFloat(float value) {
    assert(false && "Calling set on cInstArg_Label");
    (void) value;
    return false;
  }

  int AsInt();
  float AsFloat() { return (float) AsInt(); }
  std::string VerboseString() {
    std::stringstream ss;
    ss << label;
    return ss.str();
  }
};

class cInstArg_Var : public cInstArg_Base {
private:
  int var_id;
public:
  cInstArg_Var(int _id) : var_id(_id) { ; }
  ~cInstArg_Var() { ; }

  bool IsVar() { return true; }
  bool SetFloat(float value);
  std::string VerboseString() {
    std::stringstream ss;
    ss << "s" << var_id;
    return ss.str();
  }
 
  int AsInt();
  float AsFloat();
};

class cInstArg_Array : public cInstArg_Base {
private:
  int var_id;
public:
  cInstArg_Array(int _id) : var_id(_id) { ; }
  ~cInstArg_Array() { ; }

  bool SetFloat(float value) {
    assert(false && "Calling set on cInstArg_Array");
    (void) value;
    return false;
  }

  int AsInt() { return var_id; };
  float AsFloat() { return 0.0; };
  std::string VerboseString() {
    std::stringstream ss;
    ss << "a" << var_id;
    return ss.str();
  }
};

class cInstArg_Reg : public cInstArg_Base {
private:
  int reg_id;
public:
  cInstArg_Reg(int _id) : reg_id(_id) { ; }
  ~cInstArg_Reg() { ; }

  bool IsVar() { return false; }
  bool SetFloat(float value);
  std::string VerboseString() {
    std::stringstream ss;
    ss << "reg" << (char) ('A' + reg_id);
    return ss.str();
  }
 
  int AsInt();
  float AsFloat();
};

class cInstArg_IP : public cInstArg_Base {
private:
public:
  cInstArg_IP() { ; }
  ~cInstArg_IP() { ; }

  bool SetFloat(float value) {
    assert(false && "Calling set on cInstArg_IP");
    (void) value;
    return false;
  }

  bool IsVar() { return false; }
  bool SetInt(int value);
  std::string VerboseString() {
    return "IP";
  }
 
  int AsInt();
  float AsFloat();
};


/////////////
//  cInst
/////////////
  
class cInst_Base {
protected:
  cHardware * hardware;
  int line_num;
  cInstArg_Base * arg1;
  cInstArg_Base * arg2;
  cInstArg_Base * arg3;
public:
  cInst_Base(int ln, cInstArg_Base * _a1=NULL, cInstArg_Base * _a2=NULL, cInstArg_Base * _a3=NULL)
    : line_num(ln), arg1(_a1), arg2(_a2), arg3(_a3) { ; }
  ~cInst_Base() { ; }

  int GetLineNum() const { return line_num; }
  int GetNumArgs() { return (arg1?1:0)+(arg2?1:0)+(arg3?1:0); }
  cInstArg_Base * GetArg1() { return arg1; }
  cInstArg_Base * GetArg2() { return arg2; }
  cInstArg_Base * GetArg3() { return arg3; }

  std::string GetArg1String() const { return arg1 ? arg1->VerboseString() : ""; }
  std::string GetArg2String() const { return arg2 ? arg2->VerboseString() : ""; }
  std::string GetArg3String() const { return arg3 ? arg3->VerboseString() : ""; }

  std::string GetArgString(int id) const {
    if (id == 0) return arg1 ? arg1->VerboseString() : "";
    if (id == 1) return arg2 ? arg2->VerboseString() : "";
    if (id == 2) return arg3 ? arg3->VerboseString() : "";
    return "";
  }

  virtual std::string GetName() const { return "unknown"; }
  virtual int GetCost() const { return 1; }
  virtual bool Run() { return false; }
  
  void SetHardware(cHardware * _h) {
    hardware = _h;
    if (arg1 != NULL) arg1->SetHardware(_h);
    if (arg2 != NULL) arg2->SetHardware(_h);
    if (arg3 != NULL) arg3->SetHardware(_h);
  }
  
  void PrintString(const std::string & msg);
  void PrintVerbose(const std::string & out_string);
};

class cInst_VAL_COPY : public cInst_Base {
private:
public:
  cInst_VAL_COPY(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_VAL_COPY() { ; }

  std::string GetName() const { return "val_copy"; }
  static std::string GetDesc() { return "val_copy : Duplicate the value of arg1 into arg2"; }

  bool Run() {
    PrintVerbose("val_copy");
    arg2->SetFloat(arg1->AsFloat());
    return true;
  }
};

class cInst_ADD : public cInst_Base {
 private:
 public:
  cInst_ADD(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_ADD() { ; }

  std::string GetName() const { return "add"; }
  static std::string GetDesc() { return "add : Add the values of arg1 and arg2 and place the sum in arg3"; }

  bool Run() {
    PrintVerbose("add");
    arg3->SetFloat(arg1->AsFloat() + arg2->AsFloat());
    return true;
  }
};

class cInst_SUB : public cInst_Base {
private:
public:
  cInst_SUB(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_SUB() { ; }

  std::string GetName() const { return "sub"; }
  static std::string GetDesc() { return "sub : Subtract the values of arg2 from arg1 and place the difference in arg3"; }

  bool Run() {
    PrintVerbose("sub");
    arg3->SetFloat(arg1->AsFloat() - arg2->AsFloat());
    return true;
  }
};

class cInst_MULT : public cInst_Base {
private:
public:
  cInst_MULT(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_MULT() { ; }

  std::string GetName() const { return "mult"; }
  static std::string GetDesc() { return "mult : Multiply the values of arg1 and arg2 and place the product in arg3"; }

  bool Run() {
    PrintVerbose("mult");
    arg3->SetFloat(arg1->AsFloat() * arg2->AsFloat());
    return true;
  }
};

class cInst_DIV : public cInst_Base {
private:
public:
  cInst_DIV(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_DIV() { ; }

  std::string GetName() const { return "div"; }
  static std::string GetDesc() { return "div : Divide the value of arg1 by arg2 and place the floor of the ratio in arg3"; }

  bool Run();
};

class cInst_MOD : public cInst_Base {
private:
public:
  cInst_MOD(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_MOD() { ; }

  std::string GetName() const { return "mod"; }
  static std::string GetDesc() { return "mod : Divide the value of arg1 by arg2 and place the *remainder* in arg3"; }

  bool Run();
};

class cInst_TEST_LESS : public cInst_Base {
private:
public:
  cInst_TEST_LESS(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_LESS() { ; }

  std::string GetName() const { return "test_less"; }
  static std::string GetDesc() { return "test_less : If (arg1 < arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_less");
    arg3->SetFloat(arg1->AsFloat() < arg2->AsFloat());
    return true;
  }
};

class cInst_TEST_GTR : public cInst_Base {
private:
public:
  cInst_TEST_GTR(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_GTR() { ; }

  std::string GetName() const { return "test_gtr"; }
  static std::string GetDesc() { return "test_gtr : If (arg1 > arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_gtr");
    arg3->SetFloat(arg1->AsFloat() > arg2->AsFloat());
    return true;
  }
};

class cInst_TEST_EQU : public cInst_Base {
private:
public:
  cInst_TEST_EQU(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_EQU() { ; }

  std::string GetName() const { return "test_equ"; }
  static std::string GetDesc() { return "test_equ : If (arg1 == arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_equ");
    arg3->SetFloat(arg1->AsFloat() == arg2->AsFloat());
    return true;
  }
};

class cInst_TEST_NEQU : public cInst_Base {
private:
public:
  cInst_TEST_NEQU(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_NEQU() { ; }

  std::string GetName() const { return "test_nequ"; }
  static std::string GetDesc() { return "test_nequ : If (arg1 != arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_nequ");
    arg3->SetFloat(arg1->AsFloat() != arg2->AsFloat());
    return true;
  }
};

class cInst_TEST_GTE : public cInst_Base {
private:
public:
  cInst_TEST_GTE(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_GTE() { ; }

  std::string GetName() const { return "test_gte"; }
  static std::string GetDesc() { return "test_gte : If (arg1 >= arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_gte");
    arg3->SetFloat(arg1->AsFloat() >= arg2->AsFloat());
    return true;
  }
};

class cInst_TEST_LTE : public cInst_Base {
private:
public:
  cInst_TEST_LTE(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_TEST_LTE() { ; }

  std::string GetName() const { return "test_lte"; }
  static std::string GetDesc() { return "test_lte : If (arg1 <= arg2), arg3 is set to 1, else arg3 is set to 0"; }

  bool Run() {
    PrintVerbose("test_lte");
    arg3->SetFloat(arg1->AsFloat() <= arg2->AsFloat());
    return true;
  }
};

class cInst_JUMP : public cInst_Base {
private:
public:
  cInst_JUMP(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_JUMP() { ; }

  std::string GetName() const { return "jump"; }
  static std::string GetDesc() { return "jump : Jump IP to position designated by arg1"; }

  bool Run();
  
};


class cInst_JUMP_IF_0 : public cInst_Base {  
private:
public:
  cInst_JUMP_IF_0(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_JUMP_IF_0() { ; }

  std::string GetName() const { return "jump_if_0"; }
  static std::string GetDesc() { return "jump_if_0 : If arg1 == 0, Jump IP to position designated by arg2"; }

  bool Run();
};

class cInst_JUMP_IF_N0 : public cInst_Base {
private:
public:
  cInst_JUMP_IF_N0(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_JUMP_IF_N0() { ; }

  std::string GetName() const { return "jump_if_n0"; }
  static std::string GetDesc() { return "jump_if_n0 : If arg1 != 0, Jump IP to position designated by arg2"; }

  bool Run();
};

class cInst_NOP : public cInst_Base {
private:
public:
  cInst_NOP(int ln) : cInst_Base(ln) { ; }
  ~cInst_NOP() { ; }

  std::string GetName() const { return "nop"; }
  static std::string GetDesc() { return "nop : No-operation."; }
  int GetCost() const { return 0; }

  bool Run() {
    PrintVerbose("nop");
    return true;
  }
};

class cInst_RANDOM : public cInst_Base {
private:
public:
  cInst_RANDOM(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_RANDOM() { ; }

  std::string GetName() const { return "random"; }
  static std::string GetDesc() { return "random : set arg2 to a random value x, where 0 <= x < arg1."; }

  bool Run();
};

class cInst_OUT_INT : public cInst_Base {
private:
public:
  cInst_OUT_INT(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_OUT_INT() { ; }

  std::string GetName() const { return "out_int"; }
  static std::string GetDesc() { return "out_int : Print out arg1 as an integer"; }

  bool Run();
};

class cInst_OUT_FLOAT : public cInst_Base {
private:
public:
  cInst_OUT_FLOAT(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_OUT_FLOAT() { ; }

  std::string GetName() const { return "out_float"; }
  static std::string GetDesc() { return "out_float : Print out arg1 as a floating-point number"; }

  bool Run();
};

class cInst_OUT_CHAR : public cInst_Base {
private:
public:
  cInst_OUT_CHAR(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_OUT_CHAR() { ; }

  std::string GetName() const { return "out_char"; }
  static std::string GetDesc() { return "out_char : Print out arg1 as a character"; }

  bool Run();
};

class cInst_PUSH_NUM : public cInst_Base {
private:
public:
  cInst_PUSH_NUM(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_PUSH_NUM() { ; }

  std::string GetName() const { return "push"; }
  static std::string GetDesc() { return "push : Store arg1 in an internal control stack"; }

  bool Run();
};

class cInst_PUSH_ARRAY : public cInst_Base {
private:
public:
  cInst_PUSH_ARRAY(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_PUSH_ARRAY() { ; }

  std::string GetName() const { return "ar_push"; }
  static std::string GetDesc() { return "ar_push : Store array arg1 in an internal control stack"; }

  bool Run();
};

class cInst_POP_NUM : public cInst_Base {
private:
public:
  cInst_POP_NUM(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_POP_NUM() { ; }

  std::string GetName() const { return "pop"; }
  static std::string GetDesc() { return "pop : Retrieve arg1 from an internal control stack"; }

  bool Run();
};

class cInst_POP_ARRAY : public cInst_Base {
private:
public:
  cInst_POP_ARRAY(int ln, cInstArg_Base * _a1)
    : cInst_Base(ln, _a1) { ; }
  ~cInst_POP_ARRAY() { ; }

  std::string GetName() const { return "ar_pop"; }
  static std::string GetDesc() { return "ar_pop : Retrieve array arg1 from an internal control stack"; }

  bool Run();
};

class cInst_AR_GET_IDX : public cInst_Base {
private:
public:
  cInst_AR_GET_IDX(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_AR_GET_IDX() { ; }

  std::string GetName() const { return "ar_get_idx"; }
  static std::string GetDesc() { return "ar_get_idx : In array arg1, find value @ index arg2, and put result in arg3"; }

  bool Run();
};

class cInst_AR_SET_IDX : public cInst_Base {
private:
public:
  cInst_AR_SET_IDX(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2, cInstArg_Base * _a3)
    : cInst_Base(ln, _a1, _a2, _a3) { ; }
  ~cInst_AR_SET_IDX() { ; }

  std::string GetName() const { return "ar_set_idx"; }
  static std::string GetDesc() { return "ar_set_idx : In array arg1, set value @ index arg2 to value arg3"; }

  bool Run();
};

class cInst_AR_GET_SIZ : public cInst_Base {
private:
public:
  cInst_AR_GET_SIZ(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_AR_GET_SIZ() { ; }

  std::string GetName() const { return "ar_get_siz"; }
  static std::string GetDesc() { return "ar_get_siz : Calculate size of array arg1 and put result in arg2"; }

  bool Run();
};

class cInst_AR_SET_SIZ : public cInst_Base {
private:
public:
  cInst_AR_SET_SIZ(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_AR_SET_SIZ() { ; }

  std::string GetName() const { return "ar_set_siz"; }
  static std::string GetDesc() { return "ar_set_siz : Resize array arg1 to arg2"; }

  bool Run();
};

class cInst_AR_COPY : public cInst_Base {
private:
public:
  cInst_AR_COPY(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_AR_COPY() { ; }

  std::string GetName() const { return "ar_copy"; }
  static std::string GetDesc() { return "ar_copy : Duplicate the value in array arg1 to array arg2"; }

  bool Run();
};

class cInst_LOAD : public cInst_Base {
private:
public:
  cInst_LOAD(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_LOAD() { ; }

  std::string GetName() const { return "load"; }
  static std::string GetDesc() { return "load : Copy from memory position arg1 into register arg2"; }

  bool Run();
  int GetCost() const { return 100; }
};

class cInst_STORE : public cInst_Base {
private:
public:
  cInst_STORE(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_STORE() { ; }

  std::string GetName() const { return "store"; }
  static std::string GetDesc() { return "store : Copy from register arg1 into memory position arg2"; }

  bool Run();
  int GetCost() const { return 100; }
};

class cInst_MEM_COPY : public cInst_Base {
private:
public:
  cInst_MEM_COPY(int ln, cInstArg_Base * _a1, cInstArg_Base * _a2)
    : cInst_Base(ln, _a1, _a2) { ; }
  ~cInst_MEM_COPY() { ; }

  std::string GetName() const { return "mem_copy"; }
  static std::string GetDesc() { return "mem_copy : Copy from memory position arg1 to memory position arg2"; }

  bool Run();
  int GetCost() const { return 100; }
};

class cInst_DEBUG_STATUS : public cInst_Base {
private:
public:
  cInst_DEBUG_STATUS(int ln)
    : cInst_Base(ln) { ; }
  ~cInst_DEBUG_STATUS() { ; }

  std::string GetName() const { return "debug_status"; }
  static std::string GetDesc() { return "debug_status : if in debug mode, print the status of all registers and memory"; }

  bool Run();
  int GetCost() const { return 0; }
};

#endif
