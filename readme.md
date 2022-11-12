# Vernier-Arduino Shield Library

<img src="https://docs.google.com/drawings/d/e/2PACX-1vRB5RxjoWBNee_8kc4jMXvcXFli0UcuuurC4-fQFWDQrTXmBJmm2Daw0M89g1lrvTtC-KgT084eh_tP/pub?w=174&h=170" align="right"/>

This directory is intended to collect libraries of objects to control the SparkFun Vernier-Arduino Shield. I have modified and adjusted online code to build a
framework that will work with PlatformIO embedded in VSCode that will compile them to build Arduino Projects.  One goal is to provide a general toolkit that would allow students to use a high level language like python in a data analysis framework such as Jupyter, Mathematica or wxMaxima to gather data directly from Vernier sensors without having to rely on proprietary software.  The philosophy of this way of doing labs is to give students a real sense of how engineers and scientists have to think about automating the process of getting data rather than using canned magic boxes that miraculously provide finished values.

Sparkfun gave out a rich starting point for control of the Vernier Shield.  The design point here is to compartmentalize the operations so that a reasonably knowledgeable person can combine these objects into a sequence that will provide data for an experiment which can later be analyzed by other software tools.

## Philosophy
We start with the idea that the student should know something about what sensor they are connecting.  Even though there is an object for detection of a
sensor based on its resistance (old school) and room for future expansion using I2C communication (new school) we shouldn't need this object as the student should recognize and know something about their equipment (an important idea in experimental design.)

The two core operational objects **VernierDigitalSensor** and **VernierAnalogSensor** are designed to be 'non-blocking'.  I want the timing and triggering to be based on the evolution of the microsecond clock and not simply sitting and waiting for something to happen.  The programming of the arduino is designed around a iterating over a repeatedly called function called loop(). This means that we can set conditions for triggering and data acquisition but still be able to interrupt the process without restarting the machine. I realize that this isn't always necessary as rebooting an Arduino is a 2 second process but I wanted to build re-entrant capability as this leads to setting the Vernier-Arduino system up as an acquisition extension of a data analysis tool like Jupyter mentioned above. One can always subclass these objects with blocking routines.

## Bonus Features
Sparkfun saw fit to include two additional I/O abilities on their board: an LED and a Button for which there are some convenience tools created as well. Having these tools makes it simple to blink the LED 3 times as it boots, for example, and/or wait for a button press to start a measurement.  Other sensor specific objects are subclasses from the _DigitalSensor_ and _AnaogSensor_ objects outlined above.

Rough outline of the folder hierarchy.  All **example/** subfolders contain usage examples that serve as a kind of unit test for the corresponding objects.

```
|--lib/
|  ├─ VernierAnalogSensor/         # Superclass of all analog sensors [BTA]
|  |   ├─ examples/
|  |   ├─  VernierAnalogSensor.cpp
|  |   ├─  VernierAnalogSensor.h
|  |
|  ├─ VernierDigitalSensor/        # Superclass of all digital sensors [BTD]
|  |   ├─ examples/
|  |   ├─  VernierAnalogSensor.cpp
|  |   ├─  VernierAnalogSensor.h
|  |
|  ├─ VernierBlinker/              # Class that controls the LED on the shield
|  |   ├─ examples/
|  |   ├─  VernierBlinker.cpp
|  |   ├─  VernierBlinker.h
|  |
|  ├─ VernierButton/               # Class that manages the button on the shield
|  |   ├─ examples/
|  |   ├─  VernierButton.cpp
|  |   ├─  VernierButton.h
|  |
|  ├─ VernierDetect/               # Class that helps detect information about analog sensor (Not extensively tested)
|  |   ├─ examples/
|  |   ├─  VernierDetect.cpp
|  |   ├─  VernierDetect.h
|  |
|  ├─ VernierThermistor/           # subclass of BTA specific to Thermistors
|  |   ├─ examples/
|  |   ├─  VernierThermistor.cpp
|  |   ├─  VernierThermistor.h
|  |
|  ├─ Vernier1DAccelerometer/      # subclass of BTA specific to 1D accelerometers
|  |   ├─ examples/
|  |   ├─  Vernier1DAccelerometer.cpp
|  |   ├─  Vernier1DAccelerometer.h
|  |
|  ├─ VernierDiffVoltSensor/       # subclass of BTA talored to diff. volt sensor.
|  |   ├─ examples/
|  |   ├─  VernierDiffVoltage.cpp
|  |   ├─  VernierDiffVoltage.h
|  |
|  ├─ VernierVoltage/              # subclass of BTA talored to basic volt probe.
|  |   ├─ examples/
|  |   ├─  VernierVoltage.cpp
|  |   ├─  VernierVoltage.h
|  |
|  ├─ VernierPhotogate             # subclass of BTD specific to the Vernier Photogate
|  │   ├─  examples/
|  │   ├─  VernierPhotogate.cpp
|  │   └─  VernierPhotogate.h
|  ├─ ShieldCommunication/          # class that tries to implement a basic communications protocol.
|  |   ├─ examples/
|  |   ├─  ShieldCommunication.cpp
|  |   ├─  ShieldCommunication.h
|  |
|  ├─ readme.md --> THIS FILE
|
|--src/  # location where the main source is placed.
   ├─  src/VernierArduinoFirmware.cpp # doesn't need to be named this.
```

Then in `src/main.cpp` you should use:
```C++
#include <Arduino.h>
#include <Streaming.h>
#include <VernierBlinker.h> // e.g.
#include <VernierVoltage.h> // e.g.

// rest H/C/CPP code
void setup() {
   // setup the conditions, communications and other details.
}

void loop()  {
   // called repeatedly as fast as the little Arduino can fly
}

// Optional:
void serialEvent() {
   // when the serial-in buffer gets a character, an interrupt flags
   // that a character was received. This stub override is where you
   // can take action.
   // N.B. this is not run during the interrupt but just before loop()
   // is called in the master event loop outside the scope of this code.
}
```
## The Libraries
You can certainly write code directly to the Arduino Uno (the Sparkfun version) and devise direct control and reading of sensors for specific specialized needs. The purpose of the various objects is to try to consolidate the various ways to talk to the shield so it seems seamless. The other approach is to use the 'Firmware' project to load a multipurpose tool that can then be communicated with code talking over a virtual serial port through the USB connection. Typically the language of choice is an interpreted language like python running inside JupyterLab but this could be node and a custom webpage as well. 


## The Main Project
`VernierArduinoFirmware.cpp` is the source code for a controler that is meant to be communicated with by python code on a
laptop or desktop connected via a USB cable through a virtual serial connection. Depending on the computer the connection
speed can be bumped up to the 1/2 Megabaud and possibly higher. This is plenty fast for most of what we want to do. 

There is a folder called VernierShieldCommunication which contains demo software and test routines for controlling and retrieving
the data.
VernierShieldCommunication/
├── ArduinoVernierFirmware.md          # detailed documentation on how to build your own python commands
├── UnitTests
│   ├── ArduinoFirmwareClient_Test.py  # and example standalone python program
│   └── Vernier SparkFun Testbed.ipynb # a Jupyter Notebook that is shows off controlling, retrieving and displaying data in a local instance JupyterLab
└── VernierShieldCommunication.py      # class that defines the routines for communication

---
## Genreal References
* [VSCode editor](https://code.visualstudio.com/)
* [Platformio](http://platformio.org/ "Open source IoT IDE")
* [SparkFun Vernier Interface](https://github.com/sparkfun/Vernier_Interface_Shield)
