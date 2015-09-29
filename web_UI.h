#ifndef TUBE_WEB_H
#define TUBE_WEB_H

#include <emscripten.h>

#include "hardware.h"

// The following two functions are from:
// http://stackoverflow.com/questions/5056645/sorting-stdmap-using-value
template<typename A, typename B> std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
  return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B> std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
  std::multimap<B,A> dst;
  std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), flip_pair<A,B>);
  return dst;
}

class VM_UI_base {
protected:
  cHardware * hardware;

public:
  VM_UI_base() : hardware(NULL) { ; }
  virtual ~VM_UI_base() { ; }

  void UpdateCode() {
    std::string inst_bg = "#f0f0f0";
    std::string IP_bg = "#d0f0d0";

    std::stringstream ss;
    ss << "<table style=\"background-color:" << inst_bg << ";\">"
       << "<tr><th>Line</th><th>Instruction</th><th>Arg 1</th><th>Arg 2</th><th>Arg 3</th></tr>";

    const std::map<std::string, int> & label_map = hardware->GetLabelMap();

    // Reorganize label_map to be sorted by position, NOT name
    std::multimap<int, std::string> position_map = flip_map(label_map);

    // Be ready to step through position map to identify when we've hit a label!
    auto label_it = position_map.begin();

    // Step through all of the instructions in the program
    for (int inst_id = 0; inst_id < hardware->GetNumInsts(); inst_id++) {
      // Check if there is a label we need to print.
      while (label_it != position_map.end() && inst_id == label_it->first) {
        ss << "<tr style=background-color:white><td colspan=5>&nbsp;&nbsp;" << label_it->second << ":</tr>";
        label_it++;
      }

      // Instruction
      cInst_Base * inst = hardware->GetInst(inst_id);

      // Make the current instruction (at the IP) a different color.
      if (inst_id == hardware->GetIP()) ss << "<tr style=background-color:" << IP_bg << ">";
      else ss << "<tr>";      

      // Print out the information about the current instruction.
      ss << "<td>" << inst_id
         << "<td>" << inst->GetName();

      // Arguments
      for (int i = 0; i < 3; i++) {
        ss << "<td>" << inst->GetArgString(i);
      }
      ss << "</tr>";
    }

    // See if there are any final labels to include
    while (label_it != position_map.end()) {
      ss << "<tr style=background-color:white><td colspan=5>&nbsp;&nbsp;"
         << label_it->second << ":</tr>";
      label_it++;
    }

    ss << "</table>";

    // Where should we scroll to on the screen?
    // Aim for top of scroll to be a few lines above IP.
    float scroll_frac = ((float) (hardware->GetIP() - 3)) / (float) hardware->GetNumInsts();
    if (scroll_frac < 0.0) scroll_frac = 0.0;

    EM_ASM_ARGS({
        var code = Pointer_stringify($0);
        var code_obj = document.getElementById("code");
        code_obj.innerHTML = code;
        code_obj.scrollTop = $1 * code_obj.scrollHeight;
        var cycle_obj = document.getElementById("cycle_count");
        cycle_obj.innerHTML = "&nbsp;&nbsp;&nbsp;Cycles Used = " + $2;
    }, ss.str().c_str(), scroll_frac, hardware->GetExeCount());


  }

  
  void UpdateConsole() {
    EM_ASM_ARGS({
        var msg = Pointer_stringify($0);
        var console_obj = document.getElementById("console");
        console_obj.innerHTML = "<pre>" + msg + "</pre>";
        console_obj.scrollTop = console_obj.scrollHeight;
    }, hardware->GetMessages().c_str());
  }

  // The main different between hardware types is available variables
  virtual void UpdateVars() = 0;
  
  void UpdateUI() { 
    UpdateCode();
    UpdateConsole();
    UpdateVars();
  }

  void SetupHardware(cHardware * _hw)
  {
    hardware = _hw;
    UpdateUI();
  }

  void DoRestart() {
    hardware->Restart();
    UpdateUI();

    // Make sure we pause the run since we jumpped back to the beginning.
    EM_ASM({
        tube_cb.is_paused = true;
        document.getElementById("play_button").innerHTML = "Play";
    });
  }

  void DoStep() {
    hardware->RunStep();
    UpdateUI();
  }

  void DoPlay() {
    EM_ASM({
        tube_cb.is_paused = !tube_cb.is_paused;
        if (tube_cb.is_paused) document.getElementById("play_button").innerHTML = "Play";
        else {
          emkJSDoCallback(tube_cb.play_step, 0);
          document.getElementById("play_button").innerHTML = "Pause";
        }
    });
  }

  void DoEnd() {
    hardware->Run();    
    UpdateUI();

    // Make sure we pause the run since end has been reached!
    EM_ASM({
        tube_cb.is_paused = true;
        document.getElementById("play_button").innerHTML = "Play";
    });
  }

  void DoPlayStep() {
    // If we've run off the end, automatically pause.
    if (hardware->GetIP() >= hardware->GetNumInsts()) {
      EM_ASM({
          tube_cb.is_paused = true;
          document.getElementById("play_button").innerHTML = "Play";
      });
    }
    EM_ASM({
        if (tube_cb.is_paused == false) {
          emkJSDoCallback(tube_cb.step, 0);
          window.setTimeout(function() { emkJSDoCallback(tube_cb.play_step, 0); }, 250);
        }
    });
  }

};


class TubeIC_UI : public VM_UI_base{
public:
  TubeIC_UI() { ; }
  ~TubeIC_UI() { ; }

  void UpdateVars() {
    // Basic layout settings
    const std::string title_bg = "#CCCCFF";
    const int table_width = 500;
    const int col_count = 4;
    const int col_width = table_width / col_count;

    // Setup the code for the table
    std::stringstream ss;
    ss << "<table width=" << table_width << "px>"
       << "<tr style=\"background-color:#CCCCFF\"><th colspan=" << col_count
       << ">Scalar Variables</th></tr>";

    const std::map<int, cVar> & var_map = hardware->GetVarMap();
    
    // Print the scalar variables into the table.
    int var_count = 0;
    for (auto var_it = var_map.begin(); var_it != var_map.end(); var_it++) {
      if (var_count % col_count == 0) ss << "<tr>";
      ss << "<td width=" << col_width << "px>s" << var_it->first << " = " << var_it->second.AsInt();
      var_count++;
    }

    // Leave blanks to fill out the row.
    while (var_count % col_count != 0) {
      ss << "<td width=" << col_width << "px>&nbsp;";
      var_count++;
    }
    
    // Print the array variables into the table.
    ss << "<tr style=\"background-color:#CCCCFF\"><th colspan=" << col_count
       << ">Array Variables</th></tr>";

    const std::map<int, cArray> & array_map = hardware->GetArrayMap();
    
    // Print the array variables into the table.
    for (auto var_it = array_map.begin(); var_it != array_map.end(); var_it++) {
      ss << "<tr><td colspan=" << col_count << ">a" << var_it->first << " = [ ";
      for (int i = 0; i < var_it->second.GetSize(); i++) {
        if (i > 0) ss << ", ";
        ss << var_it->second.GetIndex(i);
      }
      //<< var_it->second.AsInt();
      ss << " ]</td></tr>";
    }


    ss << "</table>";

    EM_ASM_ARGS({
        var var_info = Pointer_stringify($0);
        document.getElementById("vars").innerHTML = var_info;
    }, ss.str().c_str());
  }

  
};


class tubecode_UI : public VM_UI_base{
public:
  tubecode_UI() { ; }
  ~tubecode_UI() { ; }

  void UpdateVars() {
    // Basic layout settings
    const std::string title_bg = "#CCCCFF";
    const int table_width = 500;
    const int col_count = 4;
    const int col_width = table_width / col_count;

    // Setup the code for the table
    std::stringstream ss;
    ss << "<table width=" << table_width << "px>"
       << "<tr style=\"background-color:#CCCCFF\"><th colspan=" << col_count
       << ">Registers</th></tr>";

    const std::map<int, cVar> & var_map = hardware->GetVarMap();
    
    // Print the registers into the table.
    int var_count = 0;
    for (auto var_it = var_map.begin(); var_it != var_map.end(); var_it++) {
      if (var_count % col_count == 0) ss << "<tr>";
      ss << "<td width=" << col_width << "px>reg" << ((char) ('A' + var_it->first))
         << " = " << var_it->second.AsInt();
      var_count++;
    }

    // Leave blanks to fill out the row.
    while (var_count % col_count != 0) {
      ss << "<td width=" << col_width << "px>&nbsp;";
      var_count++;
    }
    
    // Print the state of the memory into the table.
    const std::vector<float> & mem_array = hardware->GetMemArray();
    const int max_mem = hardware->GetMaxMemSet();
    const int row_size = 10;

    ss << "</table><table width=" << table_width << "px>"
       << "<tr style=\"background-color:#CCCCFF\"><th colspan=" << (row_size+1)
       << ">Memory</th></tr>";

    bool skipping = false;
    for (int i = 0; i <= max_mem; i += row_size) {
      // See if this entire row is just zeros
      bool all_zero = true;
      for (int j = i; j < i+row_size; j++) {
        if (mem_array[j] != 0) { all_zero = false; break; }
      }

      // If there is information on this row (or its the first row) print it!
      if (all_zero == false || i == 0) {
        ss << "<tr><th>" << i;
        for (int j = i; j < i+row_size; j++) {
          ss << "<td>" << mem_array[j];
        }
        ss << "</tr>";
        skipping = false;
      }

      // If this row is being skipped, put a ... unles we already have...
      else if (skipping == false) {
        ss << "<tr><th colspan=" << row_size+1 << ">...</tr>";
        skipping = true;
      }
    }

    ss << "</table>";

    EM_ASM_ARGS({
        var var_info = Pointer_stringify($0);
        document.getElementById("vars").innerHTML = var_info;
    }, ss.str().c_str());
  }

  
};


#endif
