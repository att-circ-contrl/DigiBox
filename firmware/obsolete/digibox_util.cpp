// Attention Circuits Control Laboratory - wlDigiBox project
// Utility code.
// Written by Christopher Thomas.
// Copyright (c) 2020 by Vanderbilt University. This work is licensed under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Includes

#include "digibox_includes.h"



//
// Private Constants

const char hexdigits[16] =
{ '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };



//
// Functions


// This renders an integer as hexidecimal digits into a string.
// FIXME - This is fast but unsafe. It's intended for full-speed reporting.

void WriteHex(char *buffer, uint32_t data, uint8_t digits)
{
  uint8_t didx, bidx;

  bidx = digits;
  for (didx = 0; didx < digits; didx++)
  {
    bidx--;
    buffer[bidx] = hexdigits[data & 0x0f];
    data >>= 4;
  }
}



//
// This is the end of the file.
