# Attention Circuits Control Laboratory - wlDigiBox firmware
# README documentation.
Written by Christopher Thomas.


## Important things to note:

* Arduino's development environment will auto-detect and include any file
that has a .ino, .c, .cpp, or .h extension.

* The Arduino IDE doesn't like hyphens in filenames; use underscores instead.

* I gave the Makefile a customized name to avoid confusing the Arduino IDE.



## Using the NeurAVR library and not the Arduino IDE:

To build using the NeurAVR library instead of using Arduino, use:

```
make -f Makefile.neuravr NEURAVRPATH=(path) (optional target)
```

The default target is "`clean`" followed by "`hex`".


Standard targets:

* `hex` --
Hex file suitable for flashing the wlDigiBox with.
* `clean` --
Removes binaries so that the Arduino IDE doesn't complain about them.

Debugging targets:

* `elf` --
Build the binary in ELF format but don't build a flash image.
* `asm` --
Decompile the ELF binary into assembly for debugging.

My convenience targets (which may need to be adjusted for your machine):

* `burn` --
Calls `avrdude` to write the hex image to `/dev/ttyACM0`.
* `isp` --
Calls `avrdude` to write the hex image via a usb AVR ISP mk II.
* `test` --
Calls "`cu`" to contact `/dev/ttyACM0` at 115.2 kbaud.
* `testfast` --
Calls "`cu`" to contact `/dev/ttyACM0` at 500 kbaud.

..."testu" and "testufast" contact `/dev/ttyUSB0` rather than `/dev/ttyACM0`.

These targets can safely be ignored; only "`hex`" and "`clean`" are
needed for development.

To burn a hex file to an Arduino Mega 2560 board using avrdude:

```
avrdude -c stk500 -p m2560 -P (serial device) -D -U flash:w:(hex file):i
```

...`(serial device)` is typically `/dev/ttyACM0` under Linux Mint; this
varies widely depending on OS and on OS version.

To contact a serial port using the "cu" client, use:

```
cu -l (serial device) -s (baud)
```

...`(serial device)` is per above, and `(baud)` is typically 115200.



Using the Arduino IDE:

** FIXME - This isn't currently supported for the wlDigiBox project. **

* Copy (or symlink) the project directory into your Arduino sketchbook
directory. The directory name must match the name of the top-level .ino file
(so, "digibox" for this project).

* Open and edit the project per normal.

* On my machine, under Linux, the Arduino IDE sometimes can't open the serial
port to burn the sketch. The sketch can be burned manually per the previous
section, once you've found the hex file.

* The Arduino IDE puts the hex file in:

```
/tmp/build(gibberish).tmp/(filename).hex.
```

Use "`ls -t /tmp|grep build|head -1`" to find the right build directory.


_This is the end of the file._
