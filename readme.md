VernierControl
==============

This is a collection of libraries to facilitate data acquisition by an Arduino
equipped with a Vernier Shield.  Each library file is specific to a particular
device or circumstance. The ultimate destination for these routines is to supervise
the Arduino with a python library that can be imported into a Jupyter notebook.

Structure of Library Routines
-----------------------------
There are a number of libraries for control of the Vernier Shield.  The design
of this system is predicated on the idea that the student knows what sensor
they are connecting.  So even though there is an object for detection of a
sensor based on its resistance (old school) and room for future expansion using
I2C communication (new school) we shouldn't need this as the idea is to have
the students put the components together according to their specific circumstances.

_Directory Structure_
The source code of each library should be placed in separate directory, like
"lib/private_lib/[here are source files]".
```
|--lib
|  |--VernierAnalogSensor  # Superclass of all analog sensors
|  |  |--examples
|  |  |  |- VernierTestAnalog.cpp
|  |  |- VernierAnalogSensor.cpp
|  |  |- VernierAnalogSensor.h
|  |          
|  |--VernierThermistor   # Subclass of AnalogSensor for Thermistors
|  |  |--examples
|  |  |  |- VernierTempTest.cpp
|  |  |- VernierThermistor.cpp
|  |  |- VernierThermistor.h
|  |           
|  |--Vernier1DAccelerometer  # Subclass of VernierAnalogSensor tailored for 1D Accelerometer
|  |  |--examples
|  |  |  |- VernierTest1DAcc.cpp
|  |  |- Vernier1DAccelerometer.cpp
|  |  |- Vernier1DAccelerometer.h
|  |           
|  |--VernierDiffVoltSensor  # Subclass of VernierAnalogSensor tailored for Differential Voltage
|  |  |--examples
|  |  |  |- VernierTestDiffVolt.cpp
|  |  |- VernierDiffVoltage.cpp
|  |  |- VernierDiffVoltage.h
|  |           
|  |--VernierVoltage  # Subclass of VernierAnalogSensor tailored for Voltage
|  |  |--examples
|  |  |  |- VernierTestVolt.cpp
|  |  |- VernierVoltage.cpp
|  |  |- VernierVoltage.h
|  |           
|  |--VernierBlinker  # Class that controls the light on the shield for feedback
|  |  |--examples
|  |  |  |- VernierTestBlink.cpp
|  |  |- VernierBlinker.cpp
|  |  |- VernierBlinker.h
|  |           
|  |--VernierButton  # Class that helps manage input from the button on the shield
|  |  |--examples
|  |  |  |- VernierTestButton.cpp
|  |  |- VernierButton.cpp
|  |  |- VernierButton.h
|  |           
|  |--VernierDetect  # Class that helps detect the type of analog sensor
|  |  |--examples
|  |  |- VernierDetect.cpp
|  |  |- VernierDetect.h
|  |           
|- platformio.ini
|--src
   |- main.cpp
```
_Testing and execution_
Then in `src/main.cpp` you should use:

```c++
#include <Streaming>
#include <VernierBlinker.h>
#include <VernierVoltage.h>

  // rest H/C/CPP code
setup() {}
loop()  {}
```
PlatformIO will find your libraries automatically, configure preprocessor's
include paths and build them.

More information about PlatformIO Library Dependency Finder
- http://docs.platformio.org/en/stable/librarymanager/ldf.html

Organization of Commands
------------------------

