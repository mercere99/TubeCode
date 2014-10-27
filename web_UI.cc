#include "web_UI.h"

#include <string>

#include "Callbacks.h"

extern "C" {
  extern void Tube_SetButtonCallbacks(int restart_cb, int steb_cb, int play_cb, int end_cb, int play_step_cb);
};


VM_UI_base * UI;
extern cHardware * main_hardware;

bool ParseString(const std::string &);

extern "C" int emkMain()
{
  UI = new TubeIC_UI();  // Only build once at the very beginning.

  emk::MethodCallback<VM_UI_base> * restart_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoRestart);
  emk::MethodCallback<VM_UI_base> * step_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoStep);
  emk::MethodCallback<VM_UI_base> * play_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoPlay);
  emk::MethodCallback<VM_UI_base> * end_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoEnd);
  emk::MethodCallback<VM_UI_base> * play_step_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoPlayStep);
  
  Tube_SetButtonCallbacks((int) restart_cb, (int) step_cb, (int) play_cb, (int) end_cb, (int) play_step_cb);

  main_hardware = NULL;

  return 0;
}


extern "C" int emkMain_tubecode()
{
  UI = new tubecode_UI();  // Only build once at the very beginning.

  emk::MethodCallback<VM_UI_base> * restart_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoRestart);
  emk::MethodCallback<VM_UI_base> * step_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoStep);
  emk::MethodCallback<VM_UI_base> * play_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoPlay);
  emk::MethodCallback<VM_UI_base> * end_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoEnd);
  emk::MethodCallback<VM_UI_base> * play_step_cb = new emk::MethodCallback<VM_UI_base>(UI, &VM_UI_base::DoPlayStep);
  
  Tube_SetButtonCallbacks((int) restart_cb, (int) step_cb, (int) play_cb, (int) end_cb, (int) play_step_cb);

  main_hardware = NULL;

  return 0;
}


extern "C" int emkLoadString(char * _string)
{
  // Initialize hardware object and UI.
  if (main_hardware != NULL) delete main_hardware;  // If we already have hardware, clean it up an build a new one.
  main_hardware = new cHardware();

  // Parse the input code (which will automatically load it into the main hardware.
  ParseString(_string);

  // Setup the UI with the newly loaded hardware.
  UI->SetupHardware(main_hardware);

  return 0;
}
