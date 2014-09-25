#include "inst.h"

#include <string>
#include <sstream>
#include "hardware.h"

int cInstArg_Label::AsInt()
{
  if (value == -1) {
    value = hardware->FindLabel(label);
  }
  return value;
}


bool cInstArg_Var::SetInt(int value)
{
  hardware->SetVar(var_id, value);
  return true;
}

int cInstArg_Var::AsInt()
{
  return hardware->GetVar(var_id).AsInt();
}

float cInstArg_Var::AsFloat()
{
  return hardware->GetVar(var_id).AsFloat();
}


bool cInstArg_Reg::SetInt(int value)
{
  hardware->SetVar(reg_id, value);
  return true;
}

int cInstArg_Reg::AsInt()
{
  return hardware->GetVar(reg_id).AsInt();
}

float cInstArg_Reg::AsFloat()
{
  return hardware->GetVar(reg_id).AsFloat();
}


bool cInstArg_IP::SetInt(int value)
{
  hardware->JumpIP(value);
  return true;
}

int cInstArg_IP::AsInt()
{
  return hardware->GetIP();
}

float cInstArg_IP::AsFloat()
{
  return (float) hardware->GetIP();
}


////////////////
// cInst_Base
////////////////

void cInst_Base::PrintString(const std::string & msg)
{
  hardware->PrintString(msg);
}

void cInst_Base::PrintVerbose(const std::string & out_string)
{
  hardware->PrintVerbose(out_string, arg1, arg2, arg3);
}

bool cInst_DIV::Run()
{
  PrintVerbose("div");

  if (arg2->AsInt() == 0) {
    hardware->Error("div: Division by Zero");
    return false;
  }

  arg3->SetInt(arg1->AsInt() / arg2->AsInt());
    
  return true;
}

bool cInst_MOD::Run()
{
  PrintVerbose("mod");

  if (arg2->AsInt() == 0) {
    hardware->Error("mod: Division by Zero");
    return false;
  }
  arg3->SetInt(arg1->AsInt() % arg2->AsInt());

  return true;
}

bool cInst_JUMP::Run()
{
  PrintVerbose("jump");

  hardware->JumpIP(arg1->AsInt());
  return true;
}

bool cInst_JUMP_IF_0::Run()
{
  PrintVerbose("jump_if_0");

  if (arg1->AsInt() == 0) hardware->JumpIP(arg2->AsInt());
  return true;
}

bool cInst_JUMP_IF_N0::Run()
{
  PrintVerbose("jump_if_n0");

  if (arg1->AsInt() != 0) hardware->JumpIP(arg2->AsInt());
  return true;
}

bool cInst_RANDOM::Run()
{
  PrintVerbose("random");

  int rand_max = arg1->AsInt();
  if (rand_max <= 0) {
    hardware->Error("random: must have a positive upper limit");
    return false;
  }
  arg2->SetInt(hardware->GetRandom(rand_max));
  return true;
}

bool cInst_OUT_INT::Run() 
{
  PrintVerbose("out_int");  

  *hardware << arg1->AsInt();
  return true;
}


bool cInst_OUT_FLOAT::Run()
{
  PrintVerbose("out_float");

  *hardware << arg1->AsFloat();
  return true;
}


bool cInst_OUT_CHAR::Run()
{
  PrintVerbose("out_char");

  *hardware << (char) arg1->AsInt();
  return true;
}


bool cInst_PUSH_NUM::Run()
{
  PrintVerbose("push (int)");

  hardware->PushInt(arg1->AsInt());
  return true;
}

bool cInst_PUSH_ARRAY::Run()
{
  PrintVerbose("push (array)");

  cArray & array = hardware->GetArray(arg1->AsInt());
  hardware->PushArray(array);
  return true;
}

bool cInst_POP_NUM::Run()
{
  PrintVerbose("pop (int)");

  arg1->SetInt(hardware->PopInt());
  return true;
}

bool cInst_POP_ARRAY::Run()
{
  PrintVerbose("pop (array)");

  cArray & array = hardware->GetArray(arg1->AsInt());
  array = hardware->PopArray();
  return true;
}

bool cInst_AR_GET_IDX::Run() 
{
  PrintVerbose("ar_get_idx");

  cArray & array = hardware->GetArray(arg1->AsInt());
  int index = arg2->AsInt();
  if (index < 0 || index >= array.GetSize()) {
    std::stringstream err;
    err << "ar_get_idx: Array index out of bounds (idx="
        << index << " array_size=" << array.GetSize() << ").";
    hardware->Error(err.str(), line_num);
    return false;
  }

  int out_val = array.GetIndex(index);
  arg3->SetInt(out_val);

  return true;
}

bool cInst_AR_SET_IDX::Run() 
{
  PrintVerbose("ar_set_idx");

  cArray & array = hardware->GetArray(arg1->AsInt());
  int index = arg2->AsInt();
  if (index < 0 || index >= array.GetSize()) {
    std::stringstream err;
    err << "ar_set_idx: Array index out of bounds (idx="
        << index << " array_size=" << array.GetSize() << ").";
    hardware->Error(err.str(), line_num);
    return false;
  }

  int new_val = arg3->AsInt();
  array.SetIndex(index, new_val);

  return true;
}

bool cInst_AR_GET_SIZ::Run() 
{
  PrintVerbose("ar_get_siz");

  cArray & array = hardware->GetArray(arg1->AsInt());
  arg2->SetInt(array.GetSize());

  return true;
}

bool cInst_AR_SET_SIZ::Run() 
{
  PrintVerbose("ar_set_siz");

  cArray & array = hardware->GetArray(arg1->AsInt());
  int new_size = arg2->AsInt();
  if (new_size < 0) {
    hardware->Error("ar_set_siz: Cannot set array size to a negative value");
    return false;
  }
  array.Resize(new_size);

  return true;
}

bool cInst_AR_COPY::Run() 
{
  PrintVerbose("ar_copy");

  cArray & array1 = hardware->GetArray(arg1->AsInt());
  cArray & array2 = hardware->GetArray(arg2->AsInt());

  array2 = array1;

  return true;
}

bool cInst_LOAD::Run() 
{
  PrintVerbose("load");

  int mem_value = hardware->GetMemValue(arg1->AsInt());
  arg2->SetInt(mem_value);


  return true;
}

bool cInst_STORE::Run() 
{
  PrintVerbose("store");

  hardware->SetMemValue(arg2->AsInt(), arg1->AsInt());

  return true;
}

bool cInst_MEM_COPY::Run() 
{
  PrintVerbose("mem_copy");

  int mem_value = hardware->GetMemValue(arg1->AsInt());
  hardware->SetMemValue(arg2->AsInt(), mem_value);

  return true;
}

bool cInst_DEBUG_STATUS::Run() 
{
  PrintVerbose("debug_status");

  hardware->DebugStatus();

  return true;
}

