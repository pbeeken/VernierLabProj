These are the different modules for the Vernier Frimware Project.

Note that not all the modules are used in the Firmware as some of the objects are meant for building 
and reading the probes by directly compiling and editing the Arduino's code and passing the data 
back via the serial port.

```
lib
├── readme.txt
├── ShieldCommunication                        # Communication object for the Shield Firmware Project
│   ├── examples
│   │   └── ShieldCommunicationTestEcho.cpp
│   ├── ShieldCommunicationCmds.h
│   ├── ShieldCommunication.cpp
│   └── ShieldCommunication.h
├── ShieldControl_DEP                          # Deprecated, ultra-simple control protocol
│   ├── examples
│   │   └── VernierRemoteControl.cpp
│   ├── ShieldControl.cpp
│   └── ShieldControl.h
├── Vernier1DAccelerometer                     # Reading and conversions for the Vernier 1D Accelerometer
│   ├── examples
│   │   └── VernierTest1DAcc.cpp
│   ├── Vernier1DAccelerometer.cpp
│   └── Vernier1DAccelerometer.h
├── VernierAnalogSensor                        # Generic Analog Sensor Object (use this as a base class for analog sensors)
│   ├── examples
│   │   └── VernierTestAnalogTiming.cpp
│   ├── VernierAnalogSensor.cpp
│   └── VernierAnalogSensor.h
├── VernierBlinker                             # Control the onboard blue LED for signaling 
│   ├── examples
│   │   └── VernierTestBlinker.cpp
│   ├── VernierBlinker.cpp
│   └── VernierBlinker.h
├── VernierButton                              # Reading button presses from the mounted button on the shield
│   ├── examples
│   │   └── VernierTestButton.cpp
│   ├── VernierButton.cpp
│   └── VernierButton.h
├── VernierDetect                              # Detection of sensors attached to the BTA or BTD ports (not extensively tested)
│   ├── VernierDetect.cpp
│   └── VernierDetect.h
├── VernierDiffVoltage                         # Reading and conversions for the Vernier Differential Voltage Sensor
│   ├── examples
│   │   └── VernierDiffVoltageTest.cpp
│   ├── VernierDiffVoltage.cpp
│   └── VernierDiffVoltage.h
├── VernierDigitalSensor                       # Generic Digital Sensor Object (use this as a base class for digital sensors)
│   ├── examples
│   │   └── VernierTestDigitalTiming.cpp
│   ├── VernierDigitalSensor.cpp
│   └── VernierDigitalSensor.h
├── VernierPhotogate                           # Reading and conversions for the Vernier Photogate
│   ├── examples
│   │   ├── VernierTestGatesA.cpp
│   │   ├── VernierTestGatesB.cpp
│   │   └── VernierTestGatesC.cpp
│   ├── VernierPhotogate.cpp
│   └── VernierPhotogate.h
├── VernierThermistor                          # Reading and conversions for the Vernier Temperature Probe
│   ├── examples
│   │   └── VernierTempTest.cpp
│   ├── VernierThermistor.cpp
│   └── VernierThermistor.h
└── VernierVoltage                             # Reading and conversions for the Vernier Voltage Sensor
    ├── examples
    │   └── VernierVoltageTest.cpp
    ├── VernierVoltage.cpp
    └── VernierVoltage.h
src
└── VernierArduinoFirmware.cpp                 # Main code for the Firmware Project
```
~Then in `src/main.c` you should use:~


More information about PlatformIO Library Dependency Finder
- http://docs.platformio.org/page/librarymanager/ldf.html
