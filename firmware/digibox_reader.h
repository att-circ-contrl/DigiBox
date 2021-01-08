// Attention Circuits Control Laboratory - wlDigiBox project
// Logic analyzer input code.
// Written by Christopher Thomas.
// Copyright (c) 2020 by Vanderbilt University. This work is licensed under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Enums


// Command opcodes.
enum ev_reader_op_t
{
  EV_OP_READ_STOP,
  EV_OP_READ_DIFF,
  EV_OP_READ_RATE,

  EV_OP_READ_VERB,

  EV_OP_READ_QUERY
};



//
// Global Variables

// Reader command lookup table.
extern neurapp_cmd_list_row_t cmdlist_reader[];



//
// Classes


// Event handler for the logic analyzer (reader).

class EV_LogicReader : public NeurAppEvent_Base
{
protected:

  // Operating mode (private enum).
  int mode;

  // State for difference reporting.
  bool diff_startup;
  uint16_t prev_data;

  // State for DLL sampling.
  // FIXME - DLL sampling NYI.

  // ISR-updated reporting state.
  bool need_report;
  uint32_t report_time;
  uint16_t report_data;

  // Safe copy of the reporting state.
  bool need_report_copy;
  uint32_t report_time_copy;
  uint16_t report_data_copy;

  // Report verbosity (private enum).
  int verbosity;

  // Query state.
  bool want_query;


  // Helper functions.

  void InitReaderState(int new_mode, uint16_t new_rate);


public:

  // Default constructor and destructor are fine.

  // Overridden member functions.

  PGM_P GetHelpScreen(void);
  void InitState(void);
  void HandleTick_ISR(void);
  void HandlePollHighPriority_ISR(void);
  void HandleCommand(uint8_t opcode, uint16_t arg1, uint16_t arg2);
  void SaveReportState_Fast(void);
  bool MakeReportString(neurapp_report_buf_t &buffer);
};



//
// This is the end of the file.
