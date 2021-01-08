// Attention Circuits Control Laboratory - wlDigiBox project
// Configuration values and switches.
// Written by Christopher Thomas.
// Copyright (c) 2020 by Vanderbilt University. This work is licensed under
// the Creative Commons Attribution-ShareAlike 4.0 International License.


//
// Diagnostics constants

#define VERSION_STR "20200729"

#define DEVICETYPE "Digital Box"
#define DEVICESUBTYPE "v1"



//
// Pin configuration constants


// Digital I/O pins.

// FIXME - Configuration NYI.


// LED pin.
// FIXME - Make LED depend on the compile target.

#define LED_PORT PORTG
#define LED_DIRECTION DDRG
#define LED_BIT ( (uint8_t) (1 << 5) )



//
// Host link constants

// Host link baud rate.
// 115.2 and 500.0 are reliable with a Linux client.
// 230.4 is unreliable with a 16 MHz crystal.
// 250 isn't supported by Linux.
#define HOST_BAUD 115200
//define HOST_BAUD 500000



//
// Timing constants

// CPU speed. This is with a stock Arduino Mega 2560 or Arduino Uno board.
#define CPU_SPEED 16000000ul

// Number of RTC ticks per second.
// This needs to be coarse enough for 32-bit timestamps, a round number for
// readability, and faster than our sampling rate.

// NOTE - Allow this to be overridden via "-D" when compiling.
#ifndef RTC_TICKS_PER_SECOND

// FIXME - Various rates for testing.
//define RTC_TICKS_PER_SECOND 20000ul
//define RTC_TICKS_PER_SECOND 15000ul
#define RTC_TICKS_PER_SECOND 10000ul
//define RTC_TICKS_PER_SECOND 5000ul

#endif



//
// General acquisition configuration.


// Input (logic analyzer) report verbosity.
// FIXME - Enum isn't defined yet, so have to use an integer value.
#define DEFAULT_READER_VERB 3



//
// This is the end of the file.
