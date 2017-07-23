/****************************************************************
VernierAnalogSensor
	This is the superclass designated to serve as the model for
	all other analog sensors. The best way to use this object is
   through the subclass associated with the specific sensor or
   experiment.

   This class was refactored to better match how we read the digital
   ports.  The most important thing to keep in mind is that the analog
   sensor timing is based on a specified time start where as the digital
   timer is from trigger to trigger. Another consideration is that while
   the Digital Port has only two values HIGH (+5V) and LOW(0V), the analog
   sensors can have two different ranges depending on the settings given
   to their pre-ampifier.

   Signals come into two different pins:
   1	Analog Sensor Output (-10V to +10V)
      Used with a number of Vernier voltage probes.
      This is wired through a scale and shifting op-amp
      circuit so that the Arduino can read it on a scale of 0 - 5V.
   6	Analog Sensor Output (0V to -5V)
      Primary sensor output for most analog sensors including light,
      temperature, force, pressure, pH, etc...
   You have to know which probe uses what pin.  Further, the polling of
   the data is handled is considered immediate. Special trigger Modes
   are not handled with this object largely because of the dual possibility
   of which pin the probe is operating on.

TODO: Provide an identification object to determine calibrations based on I2C
      probes.

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

	17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef VernierAnalogSensor_h
#define VernierAnalogSensor_h
#include <VernierButton.h>


enum SAMPLERATES {
   BUTTONPRESS = 0,
   S_1Hz = 1000,
   S_5Hz = 200,
   S_10Hz = 100, // 100 msec/sample
   S_20Hz = 50,
   S_40Hz = 25,
   S_50Hz = 20,
   S_100Hz = 10,
   S_200Hz = 5,
   FASTEST = 1, // 1 msec, practically: 1.5 msec per sample
};


/** Constructor
 *    setup channels and initialize values.
 */
class VernierAnalogSensor
{
  public:
       // Constructor. Mainly sets up pins.
       // N.B. you specifing a channel sets the range.
       VernierAnalogSensor( int channel );

      // read raw 'voltage' analog value. Immediate return
      int readPort();

      // set the sample time for values during polling
      void initTimer( unsigned long delTimeMS=S_10Hz );

      // intended to be placed in the loop() stub to periodically check the
      // state of the analog channel. It will react to the set of the trigger
      // condition. Returns true if a trigger condition was met.
      bool pollPort();

      // reset the timers and counters
      void          begin();       // start the clock

      // Getters for data
      const char*   getUnits() { return _units; } // return sensor's units
      unsigned long getTime() { return _absTime; }
      int           getLastRead() { return _rawReading; }


	// constants for 10 bit channels [0-1024]
   //   the 5V channel has a resolution of 5mV
   //   the 10V channel has a resolution 0f 20mV but is bipolar
    const static int BTA01_5V  = 14;  // A0 Pin6 on BTA01
    const static int BTA01_10V = 15;  // A1 Pin1 on BTA01
    const static int BTA02_5V  = 16;  // A2 Pin6 on BTA02
    const static int BTA02_10V = 17;  // A3 Pin1 on BTA02

	// elements for subclassing
  protected:
  	// default linear calibration
    virtual float applyCalibration( int adcValue );
    // default quantities for subclasses
    float        _slope;
    float        _intcpt;
    const char*  _units;

	// exclusive to this object
  private:
    VernierButton _btn;
    int           _channel;      // channel to read from
    int           _rawReading;   // last raw reading
    unsigned long _absTime;      // time of last read ofset from start
    unsigned long _trigTime;     // how long to wait between readings 0 is button push
    unsigned long _nextRead;     // time to take next reading
    unsigned long _start_us;     // marker for start sequence

};

#endif
