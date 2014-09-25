#include "hardware.h"

void cHardware::AddInst(cInst_Base * inst)
{
  inst->SetHardware(this);
  inst_vector.push_back(inst);
}

void cHardware::AddLabel(std::string _l)
{
  if (label_map.find(_l) != label_map.end()) {
    (*this) << "Warning: label '" << _l << "' being reused!" << '\n';
  }
  label_map[_l] = (int) inst_vector.size(); // The current size represents the value of the next line.
}

int cHardware::FindLabel(std::string _l)
{
  if (label_map.find(_l) == label_map.end()) {
    std::string error_str = "Unknown label '";
    error_str += _l;
    error_str += "'";
    Error(error_str);
  }
  return label_map[_l];
}


bool cHardware::RunStep()
{
  if (IP >= (int) inst_vector.size()) return false;

  advance_IP = true;  // By default, advance the instruction pointer after execution unless turned off.

  exe_count += inst_vector[IP]->GetCost();
  inst_vector[IP]->Run();
  
  if (timeout >= 0 && exe_count >= timeout) {
    (*this) << "Reached execution count limit of " << timeout << ".  Halting." << '\n';
    IP = inst_vector.size(); // Move IP to end to stop further execution.
  }
  
  if (advance_IP) IP++;

  return true;
}


bool cHardware::Run()
{
  while (IP >= 0 && IP < (int) inst_vector.size()) {
    RunStep();
  }

  if (count_cycles) (*this) << "[[ Total CPU cycles used: " << exe_count << " ]]" << '\n';

  return true;
}
