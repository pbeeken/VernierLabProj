
This directory is intended for the project specific (private) libraries.
PlatformIO will compile them to static libraries and link to executable file.

There are a number of libraries for control of the Vernier Shield.  The design
of this system is predicated on the idea that the student knows what sensor
they are connecting.  So even though there is an object for detection of a
sensor based on its resistance (old school) and room for future expansion using
I2C communication (new school) we shouldn't need this as the idea is to have
the students put the components together according to their specific circumstances.

The source code of each library should be placed in separate directory, like
"lib/private_lib/[here are source files]".
|--lib
|  |--VernierAnalogSensor
|  |  |--examples
|  |  |  |- VernierTestAnalog.cpp
|  |  |- VernierAnalogSensor.cpp
|  |  |- VernierAnalogSensor.h
|  |           # Superclass of all analog sensors
|  |--VernierThermistor
|  |  |--examples
|  |  |  |- VernierTempTest.cpp
|  |  |- VernierThermistor.cpp
|  |  |- VernierThermistor.h
|  |           # Subclass of AnalogSensor for Thermistors
|  |--Vernier1DAccelerometer
|  |  |--examples
|  |  |  |- VernierTest1DAcc.cpp
|  |  |- Vernier1DAccelerometer.cpp
|  |  |- Vernier1DAccelerometer.h
|  |           # Subclass of VernierAnalogSensor tailored for 1D Accelerometer
|  |--VernierDiffVoltSensor
|  |  |--examples
|  |  |  |- VernierTestDiffVolt.cpp
|  |  |- VernierDiffVoltage.cpp
|  |  |- VernierDiffVoltage.h
|  |           # Class that helps detect the type of analog sensor
|  |--VernierVoltage
|  |  |--examples
|  |  |  |- VernierTestVolt.cpp
|  |  |- VernierVoltage.cpp
|  |  |- VernierVoltage.h
|  |           # Class that helps detect the type of analog sensor
|  |--VernierBlinker
|  |  |--examples
|  |  |  |- VernierTestBlink.cpp
|  |  |- VernierBlinker.cpp
|  |  |- VernierBlinker.h
|  |           # Class that controls the light on the shield for feedback
|  |--VernierButton
|  |  |--examples
|  |  |  |- VernierTestButton.cpp
|  |  |- VernierButton.cpp
|  |  |- VernierButton.h
|  |           # Class that helps manage input from the button on the shield
|  |--VernierDetect
|  |  |--examples
|  |  |- VernierDetect.cpp
|  |  |- VernierDetect.h
|  |           # Class that helps detect the type of analog sensor
|  |- readme.txt --> THIS FILE
|- platformio.ini
|--src
   |- main.cpp

Then in `src/main.cpp` you should use:
__________________________________________
#include <Streaming>
#include <VernierBlinker.h>
#include <VernierVoltage.h>

// rest H/C/CPP code
setup() {}
loop()  {}
__________________________________________

PlatformIO will find your libraries automatically, configure preprocessor's
include paths and build them.

More information about PlatformIO Library Dependency Finder
- http://docs.platformio.org/en/stable/librarymanager/ldf.html
