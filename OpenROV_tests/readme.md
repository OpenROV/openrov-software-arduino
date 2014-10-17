This is the unit testing project for the OpenROV firmware.

It uses the https://github.com/mmurdoch/arduinounit as a unit testing framework.

These tests are meant to run on either the OpenROV, an Arduino, or an Ardunio simulator.

This project is incompatible with the existing Arduino IDE.  You need to use either the ino command line tools or another IDE such as viual micro.

To execute tests using [ino tools](http://inotool.org/):
------

```
cd <path>/OpenROV_tests
ino build
ino upload
ino serial -b 115200
```

Note: ino serial requires picocom to be installed.  Alternatively you can open up the serial monitor in the Arduino IDE. Also a reminder, to exit picocom is ^a^x.

You should see something similar to this:

```
Babs:OpenROV_tests brian$ ino build
src/sketch.ino
Scanning dependencies of src
src/command_tests.cpp
src/sketch.cpp
Linking firmware.elf
Converting to firmware.hex

Babs:OpenROV_tests brian$ ino upload
Guessing serial port ... /dev/tty.usbmodem1411

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f
avrdude: reading input file ".build/uno/firmware.hex"
avrdude: writing flash (3136 bytes):

Writing | ################################################## | 100% 0.51s

avrdude: 3136 bytes of flash written
avrdude: verifying flash memory against .build/uno/firmware.hex:
avrdude: load data flash data from input file .build/uno/firmware.hex:
avrdude: input file .build/uno/firmware.hex contains 3136 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.41s

avrdude: verifying ...
avrdude: 3136 bytes of flash verified

avrdude: safemode: Fuses OK

avrdude done.  Thank you.

Babs:OpenROV_tests brian$ ino serial -b 115200
Guessing serial port ... /dev/tty.usbmodem1411
picocom v1.7

port is        : /dev/tty.usbmodem1411
flowcontrol    : none
baudrate is    : 115200
parity is      : none
databits are   : 8
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
nolock is      : yes
send_cmd is    : sz -vv
receive_cmd is : rz -vv
imap is        :
omap is        :
emap is        : crcrlf,delbs,

Terminal ready

Thanks for using picocom
Babs:OpenROV_tests brian$ ino build
src/sketch.ino
Scanning dependencies of src
src/sketch.cpp
Linking firmware.elf
Converting to firmware.hex
Babs:OpenROV_tests brian$ ino upload
Guessing serial port ... /dev/tty.usbmodem1411

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f
avrdude: reading input file ".build/uno/firmware.hex"
avrdude: writing flash (9840 bytes):

Writing | ################################################## | 100% 1.58s

avrdude: 9840 bytes of flash written
avrdude: verifying flash memory against .build/uno/firmware.hex:
avrdude: load data flash data from input file .build/uno/firmware.hex:
avrdude: input file .build/uno/firmware.hex contains 9840 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 1.26s

avrdude: verifying ...
avrdude: 9840 bytes of flash verified

avrdude: safemode: Fuses OK

avrdude done.  Thank you.

Babs:OpenROV_tests brian$ ino serial -b 115200
Guessing serial port ... /dev/tty.usbmodem1411
picocom v1.7

port is        : /dev/tty.usbmodem1411
flowcontrol    : none
baudrate is    : 115200
parity is      : none
databits are   : 8
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
nolock is      : yes
send_cmd is    : sz -vv
receive_cmd is : rz -vv
imap is        :
omap is        :
emap is        : crcrlf,delbs,

Terminal ready
Assertion failed: (x=3) != (y=3), file OpenROV_UnitTests, line 17.
Test bad failed.
icmd:test4(1,2,3,4);
Test injected_command_args_0_holds_number_of_arguments passed.
icmd:test2(0);
Test injected_command_cannot_be_read_twice passed.
log: CommandBufferOverrun;
icmd:test3(1);
Test injected_command_drops_next_command_to_be_executed_if_outofbuffer passed.
icmd:test1(0);
Test injected_command_is_the_next_command_read passed.
Test ok passed.
Test summary: 5 passed, 1 failed, and 0 skipped, out of 6 test(s).
```

To add to the test:
------

In theory this framework supports creating modular tests.  Someone will need to figure out a way to seperate tests by component to help organize everything.

In the meantime,
1) include the relative path to the .h and .cpp file in the sketch.ino file.
2) add your tests with descriptive [BDD](http://en.wikipedia.org/wiki/Behavior-driven_development) style descriptions
