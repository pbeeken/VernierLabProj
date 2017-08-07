# Communication Protocol for Arduino Vernier Shield

## Introduction

This sketch/app/program is designed to run on an Arduino with a connected SparkFun Arduino shield.  In this discussion the shield/Arduino combination may be referred to as the Shield or Arduino but is recognized as a combination device. The design point is to allow a user to use python (or other language library) to communicate over the pseudo serial port usb connection to control the Arduino/Vernier Shield.  What follows is a description of the communication protocol for the shield for developers but the host interface should provide a library of commands for the student to allow them to assemble their own data acquisition systems.

## Philosophy
The idea of this project is to allow students to take charge of their data acquisition tools so they have a better connection to the sensors that are being used to gather information and the analysis. With the rise of jupyter in many engineering departments across the country as a general framework for data analysis a tool for connecting to data acquisition devices becomes clear.  This is a first attempt at providing an interface.

## Protocol
The commands fall into one of five categories:

* State changing commands
* Mode changing commands (what are we reading...)
* start and stop conditions
* Interogation
* Immediate reactions

All commands will return a single byte response if the command was understood and can be operated upon. If the return byte is '!' then the command was good otherwise a '?' will be sent.

Upon boot the Arduino initializes various parameters and then flashes the blue LED 3 times to indicate it is ready. Most of the time whenever a new connection is made through the faux serial port for the first time the Arduino reboots.  Depending on how the host program is set up this might occur every time.  A 'HALT' command can be sent to the Arduino and look for the one byte acknowledgement that the command was received and understood to programatically test if the Arduino is ready.  The Arduino is in WAIT state upon boot.

The WAIT state is when the shield is waiting for a command, unconcerned with data acquisition. The READY state is one where the shield is 'armed' waiting on some event that will begin data acquisition. The RUNNING state is when the shield is actively taking data.

### Digital Ports
There are two types of ports which demand different modes of thinking about how to get information. Digital ports gather binary states of external sensors. Here we are concerned solely about the timing of changes in state of the gates or devices.  The set up for a digital port need only answer the question of which transitions to trigger the timing. Here the only optional consideration is to decide when the timing should stop.

### Analog Ports
Analog ports measure data sources where the user must outline how often to take data. On any channel we have two choices of how to scale the data. There is an input amplifier on each channel and we have to decide whether we are monitoring the &plusmn;5V or &plusmn;10V input. Which one to use is dependent of the type of probe we are monitoring. Part of the philosophy is for users to be aware of the probe they are using. Here there are two options: one for the conditions that begin acquisition and another for when to stop.

### Immediate Commands
In addition to special timing modes there are, of course, immediate read commands that allow the caller to interrogate the current readings from the ports.

### Structure of Commands
The numerical codes chosen for the basic 1 byte commands are specially constructed so that the encode the number of parameters that must follow.  The lowest 3 bits (values from 0 to 3) indicate how many bytes are to follow the command. Most immediate commands, for example, have no additional parameters (lowest 3 bits are 0) vs analog data acquisition requires 3.

The Arduino boots with 3 blinks of the blue LED on the shield.

### State Commands:
These commands are all one byte with no additional parameters.
* HALT  .. Stop what you are doing and return to WAIT state
* GO  .. Begin Acquisition (Enter a READY, or RUNNING state, depending on trigger state)
* BTN  ..Enter GO state after button press (regardless of other settings)

### Mode changing commands (what are we going to do...)
* Digital Acquisition: takes one additional parameter

| Digital Acquisition       | Parameter One  | ~~Parameter Two~~ |
|---------------------------|:---------------|:--------------|
| `DIGMDE1 = 0x31` P1   |`LOW2HIGH=0x01` |       |
| `DIGMDE2 = 0x35` P2   |`HIGH2LOW=0x02` |  |
|                           |`ANY=0x03`      |  |
|                           |`RISING_T=0x04` |  |
|                           |`FALLING_T=0x05`| &shy; |

* Analog Acquisition: All take two parameters

| Digital Acquisition       | Parameter One  | Parameter Two |
|---------------------------|:---------------|:--------------|
| `ANAMDE15 = 0x42` P1 &plusmn;5V |`BUTTONPRESS = 0`| `IMMED=0x00` |
| `ANAMDE25 = 0x46` P2 &plusmn;5V |`S_30s = 15, S_10s = 14,`|`RISE_ABOVE=0x02`|
| `ANAMDE110 = 0x4A` P1 &plusmn;10V |`S_5s = 13, S_2s = 12`|`FALL_BELOW=0x01`|
| `ANAMDE210 = 0x4E` P2 &plusmn;10V |`S_1Hz = 11, S_5Hz = 10,`|  |
|                           |`S_10Hz = 9, S_20Hz = 8,`|High bits are|
|                           |`S_40Hz = 7, S_50Hz  = 6,`|trigger level|
|                           |`S_100Hz = 5, S_200Hz = 4,`||
|                           |`S_500Hz = 3,S_1kHz  = 2,FASTEST = 1`|&shy;|
  1 msec, practically: 1.5 msec per sample

* Start and stop conditions
  * `TRIGMODE = 0x52`
  * `STOPMODE = 0x56`
* Immediate Commands
  * `STATUS  = 0x50`
  * `IMMEDA105 = 0x60`
  * `IMMEDA205 = 0x64`
  * `IMMEDA110 = 0x68`
  * `IMMEDA210 = 0x6A`
  * `IMMEDD1 = 0x70`
  * `IMMEDD2 = 0x74`
