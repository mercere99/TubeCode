#include "web_UI.h"
#include "../Empirical/web/web.h"

#include <string>

namespace UI = emp::web;

UI::Document doc("emp_base");

VM_UI_base * VMUI;
extern cHardware * main_hardware;

void DoLoadCode(const std::string & in_code) { ; }

void DoRestart() { VMUI->DoRestart(); }
void DoStep() { VMUI->DoStep(); }
void DoPlay() { VMUI->DoPlay(); }
void DoEnd() { VMUI->DoEnd(); }

bool ParseString(const std::string &);

int main()
{
  doc << "<h1>Welcome to the TubeIC virtual machine</h1>"
      << "<p>Choose a Tube Intermediate Code file that you would like to load and run.</p>";

  //    <input type="file" id="infile" name="infile[]" />
  //    <output id="list"></output>
  doc.AddFileInput(DoLoadCode, "load_code");
  
  doc.AddButton(DoRestart, "Restart", "but_restart")
    .Title("Restart program from beginning").SetWidth(80).Disabled(true);
  doc.AddButton(DoStep, "Step", "but_step")
    .Title("Execute a single instruction in the program").SetWidth(80).Disabled(true);
  doc.AddButton(DoPlay, "Play", "but_play")
    .Title("Continuously execute instructions").SetWidth(80).Disabled(true);
  doc.AddButton(DoEnd, "To End", "but_end")
    .Title("Execute entire program and display final state").SetWidth(80).Disabled(true);

  auto code_div = doc.AddSlate("code_div");
  code_div.SetColor("black");
  code_div.SetPadding(5);
  code_div.SetFloat("left");
  code_div.SetHeight(500);
  code_div.SetOverflow("auto");

  UI::Table code_table(1,5,"code");
  code_div << code_table;
  code_table.SetCSS("border-collapse", "collapse");
  code_table.SetBackground("#F0F0F0");

  code_table.AddHeader(0,0, "Line");
  code_table.AddHeader(0,1, "Instruction");
  code_table.AddHeader(0,2, "Arg 1");
  code_table.AddHeader(0,3, "Arg 2");
  code_table.AddHeader(0,4, "Arg 3");

  // @CAO Load TubeIC for now, but base on a compilation variable.
  VMUI = new TubeIC_UI();  // Only build once at the very beginning.
  // VMUI = new tubecode_UI();  // Only build once at the very beginning.

  main_hardware = NULL;

  return 0;
}


extern "C" int LoadCode(char * _string)
{
  // Initialize hardware object and UI.
  if (main_hardware != NULL) delete main_hardware;  // Remove existing hardware, if any
  main_hardware = new cHardware();                  // Build new hardware.

  // Parse the input code (which will automatically load it into the main hardware.
  ParseString(_string);

  // Setup the UI with the newly loaded hardware.
  VMUI->SetupHardware(main_hardware);

  return 0;
}
