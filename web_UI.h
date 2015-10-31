#ifndef TUBE_WEB_H
#define TUBE_WEB_H

#include <string>
#include <emscripten.h>

#include "../Empirical/emtools/emfunctions.h"
#include "../Empirical/tools/functions.h"
#include "../Empirical/web/web.h"

#include "hardware.h"

namespace UI = emp::web;


extern "C" int LoadCode(std::string);

// Some general constants.
static const std::string inst_bg = "#f0f0f0";  // What color should general cells be?
static const std::string IP_bg = "#d0f0d0";    // What color should active cells be?
static const std::string title_bg = "#CCCCFF"; // What color should the var title background be?

static const int var_table_width = 500;
static const int var_table_col_count = 4;
static UI::Document doc("emp_base");

void DoLoadCode(const std::string & in_code) {
  emp::Alert(in_code);
  LoadCode(in_code);
  doc.Button("but_restart").Disabled(false);
  doc.Button("but_step").Disabled(false);
  doc.Button("but_play").Disabled(false);
  doc.Button("but_end").Disabled(false);
}

class VM_UI_base {
protected:
  cHardware * hardware;

  bool is_paused;
  
public:
  VM_UI_base() : hardware(NULL), is_paused(true)
  {
    doc << "<h1>Welcome to the TubeIC virtual machine</h1>"
        << "<p>Choose a Tube Intermediate Code file that you would like to load and run.</p>";
    
    doc << "<p>";
    doc.AddFileInput(DoLoadCode, "load_code");
    
    doc.AddButton([this](){DoRestart();}, "Restart", "but_restart")
      .Title("Restart program from beginning").SetWidth(80).Disabled(true);
    doc.AddButton([this](){DoStep();}, "Step", "but_step")
      .Title("Execute a single instruction in the program").SetWidth(80).Disabled(true);
    doc.AddButton([this](){DoPlay();}, "Play", "but_play")
      .Title("Continuously execute instructions").SetWidth(80).Disabled(true);
    doc.AddButton([this](){DoEnd();}, "To End", "but_end")
      .Title("Execute entire program and display final state").SetWidth(80).Disabled(true);
    
    doc << "</p>";
    auto code_div = doc.AddSlate("code_div");
    code_div.SetColor("black");
    code_div.SetPadding(5);
    code_div.SetFloat("left");
    code_div.SetHeight(500);
    code_div.SetOverflow("auto");
    
    UI::Table code_table(1,5,"code");
    code_div << code_table;
    code_table.SetCSS("border-collapse", "collapse");
    code_table.SetBackground(inst_bg);
    
    code_table.AddHeader(0,0, "Line");
    code_table.AddHeader(0,1, "Instruction");
    code_table.AddHeader(0,2, "Arg 1");
    code_table.AddHeader(0,3, "Arg 2");
    code_table.AddHeader(0,4, "Arg 3");

    UI::Slate rt_column("rt_column");
    UI::Slate console("console");
    UI::Slate var_div("var_div");
    UI::Table var_table(3, var_table_col_count, "var_table");
    var_table.SetCSS("border-collapse", "collapse");

    console.SetSize(500, 200).SetFloat("left").SetPadding(5)
      .SetColor("white").SetBackground("black")
      .SetOverflow("auto").SetBorder("4px solid white");

    var_div.SetSize(var_table_width,290).SetFloat("left").SetPadding(5)
      .SetColor("black").SetBackground("white")
      .SetOverflow("auto");

    var_table.SetWidth(var_table_width);
    var_table.GetCell(0,0).SetColSpan(var_table_col_count).SetHeader().SetBackground("#CCCCFF") << "Scalar Variables";
    var_table.GetCell(1,0).SetColSpan(var_table_col_count);
    var_table.GetCell(2,0).SetColSpan(var_table_col_count).SetHeader().SetBackground("#CCCCFF") << "Array Variables";

    var_div << var_table;
    rt_column << console;
    rt_column << var_div;
    doc << rt_column;
    
  }
  virtual ~VM_UI_base() { ; }

  void UpdateCode() {
    UI::Table code_table = doc.Table("code");

    // Reorganize label_map to be sorted by position, NOT name
    std::multimap<int, std::string> position_map = emp::flip_map(hardware->GetLabelMap());

    // Be ready to step through position map to identify when we've hit a label!
    auto label_it = position_map.begin();

    // Resize the table appropriately.
    const int num_rows = hardware->GetNumInsts() + position_map.size() + 1;
    code_table.Rows( num_rows );

    // Step through all of the instructions in the program
    int cur_row = 1;
    for (int inst_id = 0; inst_id < hardware->GetNumInsts(); inst_id++) {
      // Clear the current row.
      code_table.GetRow(cur_row).Clear();
      code_table.GetRow(cur_row).SetBackground("white");

      // Check if there is a label we need to print.
      while (label_it != position_map.end() && inst_id == label_it->first) {
        code_table.GetCell(cur_row, 0).SetColSpan(5) << label_it->second << ":";
        label_it++;
        cur_row++;
      }

      // Instruction
      cInst_Base * inst = hardware->GetInst(inst_id);

      // Make the current instruction (at the IP) a different color.
      if (inst_id == hardware->GetIP()) {
        code_table.GetRow(cur_row).SetBackground(IP_bg);
      }
        
      // Update the information about the current instruction.
      code_table.GetCell(cur_row, 0) << inst_id;
      code_table.GetCell(cur_row, 1) << inst->GetName();

      // If there are arguments, update them as well.
      for (int i = 0; i < 3; i++) {
        code_table.GetCell(cur_row, i+2) << inst->GetArgString(i);
      }

      cur_row++;
    }

    // See if there are any final labels to include
    while (label_it != position_map.end()) {
      code_table.GetCell(cur_row, 0).SetColSpan(5) << label_it->second << ":";
      label_it++;
      cur_row++;
    }

    code_table.Redraw();

    // std::stringstream ss;
    // ss << "Testing OK()" << std::endl;
    // code_table.OK(ss, false);
    // emp::Alert(ss.str());

    
    // Where should we scroll to on the screen?
    // Aim for top of scroll to be a few lines above IP.

    // float scroll_frac = ((float) (hardware->GetIP() - 3)) / (float) hardware->GetNumInsts();
    // if (scroll_frac < 0.0) scroll_frac = 0.0;

    // EM_ASM_ARGS({
    //     var code = Pointer_stringify($0);
    //     var code_obj = document.getElementById("code");
    //     code_obj.innerHTML = code;
    //     code_obj.scrollTop = $1 * code_obj.scrollHeight;
    //     var cycle_obj = document.getElementById("cycle_count");
    //     cycle_obj.innerHTML = "&nbsp;&nbsp;&nbsp;Cycles Used = " + $2;
    // }, ss.str().c_str(), scroll_frac, hardware->GetExeCount());


  }

  
  void UpdateConsole() {
    UI::Slate console = doc.Slate("console");

    console.ClearChildren();
    console << hardware->GetMessages();

    // @CAO Make sure to scroll to console bottom!
    // console_obj.scrollTop = console_obj.scrollHeight;

    console.Redraw();
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

    is_paused = true;
    doc.Button("but_play").Label("Play");
  }

  void DoStep() {
    hardware->RunStep();
    UpdateUI();
  }

  void DoPlay() {
    is_paused = !is_paused;
    if (is_paused) doc.Button("but_play").Label("Play");
    else {
      doc.Button("but_play").Label("Pause");
      DoPlayStep();
    }
  }

  void DoEnd() {
    hardware->Run();    
    UpdateUI();

    is_paused = true;
    doc.Button("but_play").Label("Play");
  }

  void DoPlayStep() {
    // If we've run off the end, automatically pause.
    if (hardware->GetIP() >= hardware->GetNumInsts()) {
      is_paused = true;
      doc.Button("but_play").Label("Play");
    }
    if (is_paused == false) {
      DoStep();
      emp::DelayCall( [this](){DoPlayStep();}, 250 );
    }
  }

};


class TubeIC_UI : public VM_UI_base{
public:
  TubeIC_UI() { ; }
  ~TubeIC_UI() { ; }

  void UpdateVars() {
    const std::map<int, cVar> & var_map = hardware->GetVarMap();
    const std::map<int, cArray> & array_map = hardware->GetArrayMap();
    
    // Basic layout settings
    const int col_width = var_table_width / var_table_col_count;
    const int num_vars = (int) var_map.size();
    const int num_arrays = (int) array_map.size();
    
    const int var_rows = (num_vars+3)/ var_table_col_count;
    const int array_rows = (num_arrays+3)/ var_table_col_count;

    const int total_rows = var_rows + array_rows + 3;

    // emp::Alert("  num_vars=", num_vars,
    //            "  var_rows=", var_rows,
    //            "  array_rows=", array_rows,
    //            "  total_rows=", total_rows);
    
    UI::Table var_table = doc.Table("var_table");
    var_table.ClearRows();
    var_table.Resize(total_rows, var_table_col_count);
    
    // Setup the header for scalars
    int cur_row = 0;
    var_table.GetRow(cur_row).SetBackground(title_bg);
    var_table.GetCell(cur_row, 0).SetColSpan(var_table_col_count).SetHeader() << "Scalar Variables";

    // Print the scalar variables into the table.
    if (var_rows) cur_row++;
    int cur_col = 0;
    for (auto var_it = var_map.begin(); var_it != var_map.end(); var_it++) {
      if (cur_col == var_table_col_count) { cur_row++; cur_col = 0; }
      var_table.GetCell(cur_row, cur_col).SetWidth(col_width)
        << "s" << var_it->first << " = " << var_it->second.AsInt();
      cur_col++;
    }

    // Fill out the rest of the current row.
    if (cur_col < var_table_col_count - 1) {
      var_table.GetCell(cur_row, cur_col).SetColSpan(var_table_col_count - cur_col);
    }
    
    // Skip a line.
    cur_row++;
    var_table.GetRow(cur_row).Clear();
    var_table.GetCell(cur_row, 0).SetColSpan(var_table_col_count) << "&nbsp;";


    // Setup header for array variables
    cur_row++;
    var_table.GetRow(cur_row).Clear().SetBackground(title_bg);
    var_table.GetCell(cur_row, 0).SetColSpan(var_table_col_count).SetHeader() << "Array Variables";

    // Print the array variables into the table.
    if (array_rows) cur_row++;
    cur_col = 0;
    for (auto var_it = array_map.begin(); var_it != array_map.end(); var_it++) {
      if (cur_col == var_table_col_count) { cur_row++; cur_col = 0; }
      var_table.GetCell(cur_row, cur_col).SetWidth(col_width)
        << "s" << var_it->first << " = [ ";
      for (int i = 0; i < var_it->second.GetSize(); i++) {
        if (i>0) var_table << ", ";
        var_table << var_it->second.GetIndex(i);
      }

      var_table << "]";
      cur_col++;
    }
    cur_row++;

    var_table.Redraw();
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
