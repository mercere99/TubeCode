#include "web_UI.h"

#include <string>

#include "Callbacks.h"

extern "C" {
  extern void Tube_SetButtonCallbacks(int restart_cb, int steb_cb, int play_cb, int end_cb, int play_step_cb);
};


TubeIC_UI * UI;
extern cHardware * main_hardware;

bool ParseString(const std::string &);

extern "C" int emkMain()
{
  UI = new TubeIC_UI();  // Only build once at the very beginning.

  emk::MethodCallback<TubeIC_UI> * restart_cb = new emk::MethodCallback<TubeIC_UI>(UI, &TubeIC_UI::DoRestart);
  emk::MethodCallback<TubeIC_UI> * step_cb = new emk::MethodCallback<TubeIC_UI>(UI, &TubeIC_UI::DoStep);
  emk::MethodCallback<TubeIC_UI> * play_cb = new emk::MethodCallback<TubeIC_UI>(UI, &TubeIC_UI::DoPlay);
  emk::MethodCallback<TubeIC_UI> * end_cb = new emk::MethodCallback<TubeIC_UI>(UI, &TubeIC_UI::DoEnd);
  emk::MethodCallback<TubeIC_UI> * play_step_cb = new emk::MethodCallback<TubeIC_UI>(UI, &TubeIC_UI::DoPlayStep);
  
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
