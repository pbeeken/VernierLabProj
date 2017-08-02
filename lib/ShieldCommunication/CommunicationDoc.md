# Communication Protocol for Arduino Vernier

This sketch/app/program is designed to run on an Arduino with a connected
SparkFun Arduino shield.  The user is intended to use a python (or other language library) to communicate over the pseudo serial port usb connection to control the Arduino/Vernier Shield.  What follows is a description of the communication protocol for the shield.

The commands fall into one of four categories:
* State changing commands
** HALT = 0x2C, GO   = 0x28, BTN  = 0x24,

* Mode changine commands (what are we reading...)
**   DIGMDE1 = 0x31, DIGMDE2 = 0x35,  // two digital ports
**   ANAMDE15 = 0x42,ANAMDE25 = 0x46, // 5 volt inputs
**   ANAMDE110 = 0x4A,ANAMDE210=0x4E,  // 10 volt inputs

* start and stop conditions
**   TRIGMODE = 0x52, STOPMODE = 0x56,

* Interogation
**   STATUS  = 0x50,

* Get immediate reads
**   IMMEDA105 = 0x60, IMMEDA205 = 0x64,
**   IMMEDA110 = 0x68, IMMEDA210 = 0x6A,
**   IMMEDD1 = 0x70,   IMMEDD2 = 0x74,

All commands will return a single byte response if the command was understood and can be operated upon. If the return byte is '!' then the command was good otherwise a '?' will be sent.

Upon boot the Arduino initializes various parameters and then flashes the blue LED 3 times to indicate it is ready. Most of the time whenever a new connection is made through the faux serial port for the first time the Arduino reboots.  Depending on how the host program is set up this might occur every time.  A 'HALT' command can be sent to the Arduino and look for the one byte acknowledgement that the command was received and understood to programatically test if the Arduino is ready.  The arduino is in WAIT state upon boot.

The shield is either waiting for a command and is unconcerned with data acquisition (WAIT).  The second state is that the shield is 'armed' waiting on some event that will begin data acquisition (READY) and the final state is where the device is actively acquiring data (RUNNING).

There are two types of ports which demand different modes of thinking about how to get information. Digital ports gather, well, binary states of external sensors. Here we are concerned solely about the timing of changes in state of the gates or devices.  The set up for a digital port need only answer the question of which transitions to trigger the timing. Here the only optional consideration is to decide when the timing should stop.

Analog ports are data sources where the user must outline how often to take data. On each channel we have two choices of how to scale the data. There is an input amplifier on each channel and we have to decide whether we are monitoring the &plusmn;5V or &plusmn;10V input. Which one to use is dependent of the type of probe we are monitoring.

In addition to special timing modes there are, of course, immediate read commands that allow the caller to interrogate the current values. The numerical codes chosen for the basic 1 byte commands are specially constructed so that the encode the number of parameters that must follow.  The lowest 3 bits (values from 0 to 3) indicate how many bytes are to follow the command. Most immediate commands, for example, have no additional parameters (lowest 3 bits are 0) vs analog data acquisition requires 3.

The Arduino boots with 3 blinks of the blue LED on the shield.

State Commands:
*HALT
..Stop what you are doing and return to wait state.
*GO
..Begin Acquisition.
*BTN
..Start Acquisition after a button press.


* Mode changine commands (what are we reading...)
**   DIGMDE1 = 0x31, DIGMDE2 = 0x35,  // two digital ports
**   ANAMDE15 = 0x42,ANAMDE25 = 0x46, // 5 volt inputs
**   ANAMDE110 = 0x4A,ANAMDE210=0x4E,  // 10 volt inputs

* start and stop conditions
**   TRIGMODE = 0x52, STOPMODE = 0x56,

* Interogation
**   STATUS  = 0x50,

* Get immediate reads
**   IMMEDA105 = 0x60, IMMEDA205 = 0x64,
**   IMMEDA110 = 0x68, IMMEDA210 = 0x6A,
**   IMMEDD1 = 0x70,   IMMEDD2 = 0x74,
