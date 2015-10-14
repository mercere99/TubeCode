#include "web_UI.h"

VM_UI_base * VMUI;
extern cHardware * main_hardware;

bool ParseString(const std::string &);

int main()
{
  // @CAO Load TubeIC for now, but base on a compilation variable.
  VMUI = new TubeIC_UI();  // Only build once at the very beginning.
  // VMUI = new tubecode_UI();  // Only build once at the very beginning.

  main_hardware = NULL;

  return 0;
}


extern "C" int LoadCode(std::string in_code)
{
  // Initialize hardware object and UI.
  if (main_hardware != NULL) delete main_hardware;  // Remove existing hardware, if any
  main_hardware = new cHardware();                  // Build new hardware.

  // Parse the input code (which will automatically load it into the main hardware.
  ParseString(in_code.c_str());

  // Setup the UI with the newly loaded hardware.
  VMUI->SetupHardware(main_hardware);

  return 0;
}
