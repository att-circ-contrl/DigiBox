// Attention Circuits Control Laboratory - wlDigiBox project
// Main entry-point.
// Written by Christopher Thomas.
// Copyright (c) 2020 by Vanderbilt University. This work is licensed under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "digibox_includes.h"



//
// Private Global Variables


// Event handler instances.

EV_LogicReader handler_reader;


// Event handler lookup table.
// Handlers are called in the order listed, so order matters.
// List producers ahead of consumers to avoid starvation.

neurapp_event_handler_row_t event_lut[] =
{
  // Handlers with multiple command lists are listed once for each list.
  // NOTE - These duplicate listings must be adjacent, so that the
  // application framework recognizes them as duplicates.

  { &handler_reader, cmdlist_reader },

  // End-of-list marker.
  { NULL, NULL }
};


// Message lookup table.

// NOTE - PSTR() only works inside a function. This is the global equivalent.
const char versionstr[] PROGMEM =
  "devicetype: " DEVICETYPE
  "  subtype: " DEVICESUBTYPE
  "  revision: " VERSION_STR
  "\r\n";

const char helpscreenstr[] PROGMEM =
  DEVICETYPE " " DEVICESUBTYPE " revision " VERSION_STR "\r\n"
  "\r\n"
  "This device acts as a logic analyzer and digital pattern generator.\r\n"
  ;

neurapp_messagedefs_t messages =
{
  versionstr,
  helpscreenstr
};


// Application framework instance.

NeurApp_Base app;



//
// Functions

// Timer ISR callback.

void TimerCallback(void)
{
  // NOTE - Reentrant ISR calls are already handled by the application
  // framework. This is necessary for UART handling and other fast events.
  // All we have to do is call the ISR update hook.

  app.DoUpdate_ISR();
}



// Virtual function error handler.
// This should never be called unless there's a critical bug.

extern "C" void __cxa_pure_virtual(void)
{
  // Recommended implementation is to hang.
  while (1)
    ;
}



//
// Main Program


int main(void)
{
  //
  // One-time initialization.

  MCU_Init();
  UART_Init(CPU_SPEED, HOST_BAUD);


  // Manually configure GPIO ports.
  // FIXME - We should make this configurable later.

  // The 8-bit port is for pattern generation.
  IO8_SelectOutputs(0xff);

  // The 16-bit port is for input.
  // Enable pull-ups to deal with floating lines.
  IO16_SelectOutputs(0x0000);
  IO16_SetPullups(0xffff);


  // Call the application framework's setup hooks.
  app.DoInitialSetup(messages, event_lut);

  // Do setup that the app framework didn't handle.
  // FIXME - NYI (nothing to do for now).

  // Do this as the last part of setup, as it starts timer ISR calls.
  Timer_Init(CPU_SPEED, RTC_TICKS_PER_SECOND);
  Timer_RegisterCallback(&TimerCallback);


  //
  // Loop forever.

  while (1)
  {
    app.DoPolling();
  }


  // We should never reach here.
  // Report success.
  return 0;
}



//
// This is the end of the file.
