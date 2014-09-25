#ifndef HARDWARE_H
#define HARDWARE_H

#include <fstream>
#include <map>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>

#include "inst.h"

class cVar {
private:
  union VarVals {
    int i;
    float f;
  } value;
public:
  cVar() { value.i = -1; }
  cVar(const cVar & _in) { value.i = _in.value.i; }
  ~cVar() { ; }

  int AsInt() const { return value.i; }
  float AsFloat() const { return value.f; }

  void Set(int _v) { value.i = _v; }
  void Set(float _v) { value.f = _v; }
};

class cArray {
private:
  std::vector<cVar> array_data;
public:
  cArray() { ; }
  cArray(const cArray & _in) { array_data = _in.array_data; }
  ~cArray() { ; }

  cArray & operator=(const cArray & _in) { array_data = _in.array_data; return *this; }

  int GetSize() { return (int) array_data.size(); }
  int GetIndex(int idx) { return array_data[idx].AsInt(); }

  void SetIndex(int idx, int value) { array_data[idx].Set(value); }
  void Resize(int new_size) { array_data.resize(new_size); }
};

class cStackEntry {
private:
  int value;
  cArray ar_value;
  bool is_array;
public:
  cStackEntry(int _v) : value(_v), is_array(false) { ; }
  cStackEntry(const cArray & _v) : ar_value(_v), is_array(true) { ; }
  ~cStackEntry() { ; }

  int AsInt() { return value; }
  const cArray & AsArray() { return ar_value; }
  bool IsArray() { return is_array; }
};

class cHardware {
private:
  std::map<std::string,int> label_map;  // Tracking positions of all labels in the source file.
  std::map<int,cVar> var_map;
  std::map<int,cArray> array_map;
  std::vector<cInst_Base *> inst_vector;
  std::vector<int> mem_array;

  std::vector<cStackEntry *> exe_stack;

  int IP;          // Instruction pointer -- which instruction to be executed?
  bool advance_IP; // Should the instruction pointer be advanced after execution?

  int exe_count;   // Number of instructions executed thus far.
  int timeout;     // Maximum number of instructions executed before halting.

  bool print_to_console;  // Should string outputs be sent to the cout and console?
  bool print_internal;    // Should printed messages be saved internally?
  std::stringstream iout; // Place to save "internal output"
  bool count_cycles;      // Should we keep track of how many CPU cycles have been used?
  bool verbose;           // Should we print information about each line executed?
  std::ofstream v_file;   // Verbose file.
public:
  cHardware() : mem_array(1<<16), IP(0), advance_IP(false), exe_count(0), timeout(-1)
              , print_to_console(true), print_internal(true), count_cycles(false), verbose(false)
  {
    // srand(time(NULL));
    srand(1);
    // iout << "Console Output:" << std::endl;
  }
  ~cHardware() { ; }

  const std::map<std::string,int> & GetLabelMap() { return label_map; }

  int GetNumInsts() const { return (int) inst_vector.size(); }
  cInst_Base * GetInst(int id) { return inst_vector[id]; }

  void AddInst(cInst_Base * inst);
  void AddLabel(std::string _l);

  int FindLabel(std::string _l);
  int GetRandom(int rand_max) {
    return rand() % rand_max;
  }

  cVar GetVar(int id) { return var_map[id]; }
  const std::map<int,cVar> & GetVarMap() { return var_map; }
  void SetVar(int id, int value) { var_map[id].Set(value); }
  void SetVar(int id, float value) { var_map[id].Set(value); }

  cArray & GetArray(int id) { return array_map[id]; }

  void PushInt(int value) { exe_stack.push_back(new cStackEntry(value)); }
  void PushArray(const cArray & value) { exe_stack.push_back(new cStackEntry(value)); }
  int PopInt() {
    if (exe_stack.size() == 0) {
      Error("Attempting to pop off an empty stack.");
      return 0;
    }
    if (exe_stack.back()->IsArray() == true) {
      Error("Popping an array off the stack, but attempting to store it in an int.");
      return 0;
    }

    int out_val = exe_stack.back()->AsInt();
    delete exe_stack.back();
    exe_stack.pop_back();
    return out_val;
  }
  cArray PopArray() {
    if (exe_stack.size() == 0) {
      Error("Attempting to pop off an empty stack.");
      return cArray();
    }
    if (exe_stack.back()->IsArray() == false) {
      Error("Popping an int off the stack, but attempting to store it in an array.");
      return cArray();
    }

    cArray out_val = exe_stack.back()->AsArray();
    delete exe_stack.back();
    exe_stack.pop_back();
    return out_val;
  }

  void Error(std::string msg, int line_num=-1) {
    if (line_num == -1) (*this) << "ERROR: " << msg << '\n';
    else (*this) << "ERROR(line " << line_num << "): " << msg << '\n';
  }
  
  int GetMemValue(int mem_pos) {
    if (mem_pos < 0) {
      Error("Cannot index into a negative memory position");
      exit(1);
    }
    if (mem_pos >= (int) mem_array.size()) {
      std::stringstream ss;
      ss << "Limit of " << mem_array.size() << " memory positions available.";
      Error(ss.str());
      exit(1);
    }
    return mem_array[mem_pos];
  }
  
  void SetMemValue(int mem_pos, int value) {
    if (mem_pos < 0) {
      Error("Cannot index into a negative memory position");
      exit(1);
    }
    if (mem_pos >= (int) mem_array.size()) {
      std::stringstream ss;
      ss << "Limit of " << mem_array.size() << " memory positions available.";
      Error(ss.str());
      exit(1);
    }
    mem_array[mem_pos] = value;
  }
  
  bool RunStep();
  bool Run();

  void Restart() {
    IP = 0;
    advance_IP = false;
    exe_count = 0;

    for (int i = 0; i < (int) mem_array.size(); i++) mem_array[i] = 0;  // Is this needed?
    var_map.clear();
    array_map.clear();
    exe_stack.clear();

    // Clear the internal record of output.
    iout.clear();
    iout.str("");
  }

  int GetIP() { return IP; }
  void JumpIP(int new_pos) { IP = new_pos; advance_IP = false; }

  void SetTimeout(int _to) { timeout = _to; }
  void CountCPUCycles() { count_cycles = true; }

  // A simple method to print strings in the correct place.
  void PrintString(const std::string & msg) {
    if (print_to_console) std::cout << msg;
    if (print_internal) iout << msg;
  }

  // Operator overloading to simply print strings in the correct place.
  cHardware & operator<<(const std::string & msg) {
    if (print_to_console) std::cout << msg;
    if (print_internal) iout << msg;
    return *this;
  }

  cHardware & operator<<(char msg) {
    if (print_to_console) std::cout << msg;
    if (print_internal) iout << msg;
    return *this;
  }

  cHardware & operator<<(int msg) {
    if (print_to_console) std::cout << msg;
    if (print_internal) iout << msg;
    return *this;
  }

  cHardware & operator<<(float msg) {
    if (print_to_console) std::cout << msg;
    if (print_internal) iout << msg;
    return *this;
  }

  inline std::string GetMessages() {
    return iout.str();
  }


  void SetVerbose() {
    if (verbose) return;
    verbose = true;
    v_file.open("trace.dat");
  }
  void PrintVerbose(const std::string & out_string, cInstArg_Base * arg1=NULL, cInstArg_Base * arg2=NULL, cInstArg_Base * arg3=NULL) {
    if (verbose==true) {
      v_file << ":: " << IP << " :: " << out_string;
      int cur_int = -1;
      if (arg1) {
        cur_int = arg1->AsInt();
        v_file << " " << arg1->VerboseString() << "(" << cur_int << ")";
      }
      if (arg2) {
        cur_int = arg2->AsInt();
        v_file << " " << arg2->VerboseString() << "(" << cur_int << ")";
      }
      if (arg3) {
        cur_int = arg3->AsInt();
        v_file << " " << arg3->VerboseString() << "(" << cur_int << ")";
      }
      v_file << std::endl;
    }
  }

  void DebugStatus() {
    if (verbose == true) {
      for (int i = 0; i < 8; i++) {
        v_file << "reg" << (char) ('A' + i) << "=" << var_map[i].AsInt() << "  ";
      }
      v_file << "IP=" << IP << std::endl;

      v_file << "Used Mem: ";
      for (int i = 0; i < (int) mem_array.size(); i++) {
        if (mem_array[i] == 0) continue;
        v_file << i << ":" << mem_array[i] << " ";
      }
      v_file << std::endl;
    }
  }
};

#endif
