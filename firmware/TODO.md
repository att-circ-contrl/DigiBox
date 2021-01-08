# Attention Circuits Control Laboratory - wlDigiBox firmware
# Change log, bug list, and feature request list.
Written by Christopher Thomas.


## Bugs and feature requests:

* Add sampling mode for reading inputs (edge detection + delay-locked loop).

* Add pattern generation for outputs (one-shot/repeated, codes/bits).
Emitted codes/pulses can stay on until the next one or can be asserted for
a specific length of time at intervals (duration vs interval distinction).
This is a superset of timer and event code functionality.

* Write up a user manual.

* Write up a developer manual to walk through this as a tutorial case for
the NeurAVR application framework.


## Low priority feature requests:

* Allow inputs and outputs to be mapped to arbitrary pins.

* Allow trigger-on-pattern or on bits to have output triggered by input.

* Allow record-on-trigger to capture fast data to memory.

* Make compile targets for the m2560 and the m328p.

* Make a shim to allow compiling with the Arduino IDE.


## Abbreviated changelog (most recent changes first):

* 29 Jul 2020 --
Moved fast hex printing to the NeurAVR library.

* 28 Jul 2020 --
Fixed a firehose bug for difference reporting.

* 27 Jul 2020 --
Added ticks-per-second to LQY output.

* 22 Jul 2020 --
Reading inputs and reporting changes implemented.

* 20 Jul 2020 --
Initial skeleton.


_This is the end of the file._
