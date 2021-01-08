// Attention Circuits Control Laboratory - wlDigiBox project
// Logic analyzer input code.
// Written by Christopher Thomas.
// Copyright (c) 2020 by Vanderbilt University. This work is licensed under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "digibox_includes.h"



//
// Private Enums

// Report verbosity.
enum reader_report_verb_t
{
  READER_VERB_NONE  = 0,
  READER_VERB_SHORT = 1,
  READER_VERB_MED   = 2,
  READER_VERB_FULL  = 3
};

// Reading mode.
enum reader_mode_t
{
  READER_MODE_OFF = 0,
  READER_MODE_DIFF,
  READER_MODE_RATE
};



//
// Global Variables

// These are actually constants, without the "const" keyword.


// Command-name instances.

neurapp_cmdname_t cmd_reader_stop = { 'L', 'S', 'T' };
neurapp_cmdname_t cmd_reader_diff = { 'L', 'C', 'H' };
neurapp_cmdname_t cmd_reader_rate = { 'L', 'R', 'A' };

neurapp_cmdname_t cmd_reader_verb = { 'L', 'V', 'B' };

neurapp_cmdname_t cmd_reader_query = { 'L', 'Q', 'Y' };


// Help screen for reader commands.
// NOTE - PSTR() only works inside a function. This is the global equivalent.

const char reader_help[] PROGMEM =
"Logic analyzer commands:\r\n"
"\r\n"
"LST  :  Stop reading.\r\n"
"LCH  :  Start reading, reporting input changes.\r\n"
"LRA n:  Start reading, reporting n samples per second.\r\n"
"LVB n:  Set report verbosity. 0 = none, 1 = short, 2 = medium, 3 = full.\r\n"
"LQY  :  Query logic analyzer configuration.\r\n"
  ;


// Reader command lookup table.

neurapp_cmd_list_row_t cmdlist_reader[] =
{
  { cmd_reader_stop, EV_OP_READ_STOP, 0 },
  { cmd_reader_diff, EV_OP_READ_DIFF, 0 },
  { cmd_reader_rate, EV_OP_READ_RATE, 1 },

  { cmd_reader_verb, EV_OP_READ_VERB, 1 },

  { cmd_reader_query, EV_OP_READ_QUERY, 0 },

  // End-of-list marker.
  { cmd_reader_query, 0, -1 }
};



//
// Classes


// Initializes reader state variables, with locking.
// Sampling rate is ignored unless we're switching to DLL sampling mode.

void EV_LogicReader::InitReaderState(int new_mode, uint16_t new_rate)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // Set to "off" (safe condition).

    mode = READER_MODE_OFF;

    diff_startup = true;
    prev_data = 0;

    need_report = false;
    report_time = 0;
    report_data = 0;


    // Enable the new mode if one was requested.

    switch(new_mode)
    {
      case READER_MODE_DIFF:
        mode = READER_MODE_DIFF;
        diff_startup = true;
        break;

      case READER_MODE_RATE:
        // FIXME - DLL sampling NYI.
        // Leave mode as "off".
        break;

      default:
        // Leave mode as "off".
        break;
    }
  }
}



// Returns a help screen describing handler-specific commands.
// This just wraps the relevant constant string.

PGM_P EV_LogicReader::GetHelpScreen(void)
{
  return reader_help;
}



// This performs internal state initialization. Multiple calls are ok.

void EV_LogicReader::InitState(void)
{
  // This updates ISR-modified state, but does locking for us.

  InitReaderState(READER_MODE_OFF, 0);


  // This state is not updated by ISRs, so we don't need a lock.

  need_report_copy = 0;
  report_time_copy = 0;
  report_data_copy = 0;

  verbosity = DEFAULT_READER_VERB;

  want_query = false;
}



// This is called from the timer ISR.
// This must complete within one tick.

void EV_LogicReader::HandleTick_ISR(void)
{
  uint32_t thistime;
  uint16_t thisdata;

  thistime = Timer_Query_ISR();
  thisdata = IO16_ReadData();

  switch (mode)
  {
    case READER_MODE_DIFF:
      // We might already have a pending report; don't squash it unless
      // we actually have new data.
      if (diff_startup || (thisdata != prev_data))
      {
        diff_startup = false;

        need_report = true;
        report_time = thistime;
        report_data = thisdata;

        prev_data = thisdata;
      }
      break;

    case READER_MODE_RATE:
      // FIXME - DLL sampling NYI.
      // New samples get queued here; the HPP handler does delay locking.
      need_report = false;
      break;

    default:
      // Assume "off".
      need_report = false;
      break;
  }
}



// This is called from the timer ISR.
// It can take multiple ticks to complete.

void EV_LogicReader::HandlePollHighPriority_ISR(void)
{
  // FIXME - DLL sampling NYI.
}



// This is called to handle user commands.
// Opcodes with fewer than two arguments have extra args set to zero.

void EV_LogicReader::HandleCommand(uint8_t opcode,
  uint16_t arg1, uint16_t arg2)
{
  switch (opcode)
  {
    case EV_OP_READ_STOP:
    case EV_OP_READ_DIFF:
    case EV_OP_READ_RATE:
      // Argument is 0 _and_ ignored if we aren't setting a sampling rate.
      InitReaderState(opcode, arg1);
      break;

    case EV_OP_READ_VERB:
      if (READER_VERB_FULL >= arg1)
        verbosity = (int) arg1;
      break;

    case EV_OP_READ_QUERY:
      want_query = true;
      break;

    default:
      // Unrecognized command. Silently do nothing.
      break;
  }
}



// This is called atomically. It copies volatile state so that reports
// can be generated.

void EV_LogicReader::SaveReportState_Fast(void)
{
  need_report_copy = need_report;
  if (need_report_copy)
  {
    need_report = false;
    report_time_copy = report_time;
    report_data_copy = report_data;
  }
}



// This is called from the polling loop to generate report text.
// It returns true if an event report was generated, false otherwise.

bool EV_LogicReader::MakeReportString(neurapp_report_buf_t &buffer)
{
  bool have_msg;

  have_msg = false;

  // Volatile state was copied by SaveReportState_Fast(), so no locking is
  // necessary.

  if (need_report_copy)
  {
    have_msg = true;
    need_report_copy = false;

    switch (verbosity)
    {
      case READER_VERB_FULL:
        // FIXME - This has to be fast, but if we make a mistake we'll get
        // an unterminated string or a buffer overrun.
        // Keep the template in SRAM rather than program memory, for speed.
        strncpy(buffer, "+ttttttttxxxx\r\n", NEURAPP_REPORT_BUFFER_CHARS);
        UTIL_WriteHex(buffer + 1, report_time_copy, 8);
        UTIL_WriteHex(buffer + 9, report_data_copy, 4);
        break;

      case READER_VERB_MED:
        // FIXME - This has to be fast, but if we make a mistake we'll get
        // an unterminated string or a buffer overrun.
        // Keep the template in SRAM rather than program memory, for speed.
        strncpy(buffer, "+ttttttxxxx\r\n", NEURAPP_REPORT_BUFFER_CHARS);
        UTIL_WriteHex(buffer + 1, report_time_copy, 6);
        UTIL_WriteHex(buffer + 7, report_data_copy, 4);
        break;

      case READER_VERB_SHORT:
        // FIXME - This has to be fast, but if we make a mistake we'll get
        // an unterminated string or a buffer overrun.
        // Keep the template in SRAM rather than program memory, for speed.
        strncpy(buffer, "+ttttxxxx\r\n", NEURAPP_REPORT_BUFFER_CHARS);
        UTIL_WriteHex(buffer + 1, report_time_copy, 4);
        UTIL_WriteHex(buffer + 5, report_data_copy, 4);
        break;

      default:
        // Assume "none".
        have_msg = false;
        break;
    }
  }

  if ((!have_msg) && want_query)
  {
    have_msg = true;
    want_query = false;

    snprintf_P( buffer, NEURAPP_REPORT_BUFFER_CHARS,
      PSTR("Read mode:  %s    Verbosity:  %s    Ticks/sec:  %lu\r\n"),
      ( (READER_MODE_DIFF == mode) ? "diff" :
        ((READER_MODE_RATE == mode) ? "rate" : " off")
      ),
      ( (READER_VERB_FULL == verbosity) ? " full" :
        ( (READER_VERB_MED == verbosity) ? "  med" :
          ((READER_VERB_SHORT == verbosity) ? "short" : "  off")
        )
      ),
      (unsigned long) RTC_TICKS_PER_SECOND
    );
  }

  return have_msg;
}



//
// This is the end of the file.
