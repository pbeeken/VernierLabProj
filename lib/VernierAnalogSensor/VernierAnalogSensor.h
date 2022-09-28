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
#include <Arduino.h>
#include <VernierButton.h>

// Sample Rates 32 values available (bottom 5 bits of first parameter)
namespace SAMPLERATES {
  const int   BUTTONPRESS = 0;
   const int  S_30s = 15;
   const int  S_10s = 14;
   const int  S_5s = 13;
   const int  S_2s = 12;
   const int  S_1Hz = 11;
   const int  S_5Hz = 10;
   const int  S_10Hz = 9; // 100 msec/sample
   const int  S_20Hz = 8;
   const int  S_40Hz = 7;
   const int  S_50Hz = 6;
   const int  S_100Hz = 5;
   const int  S_200Hz = 4;
   const int  S_500Hz = 3;
   const int  S_1kHz = 2;
   const int  FASTEST = 1; // 1 msec, practically: 1.5 msec per sample
};

namespace ATRIGCOND {
  //      const static int TS_BUTTON     = 0x03;  // FUTURE, UNUSED CURRENTLY
  const int TS_RISE_ABOVE = 0x02;
  const int TS_FALL_BELOW = 0x01;
  const int TS_IMMEDIATE  = 0x00;
};

namespace STATE {
  const int TS_HALT       = 0x00;
  const int TS_ARMED      = 0x10;
  const int TS_RUN        = 0x20;
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
      void setSampleRate( char intFlag=SAMPLERATES::S_10Hz );

      // set trigger condition (upon arming)
      void setTrigger( int trigCond=ATRIGCOND::TS_IMMEDIATE, int raw_value=512 );

      // set stop conditions by #points. To set by time then simply divide
      // the time by the sample rate.
      // e.g. for 10sec at 10Hz pass: 10[sec] * 10 [samples/sec]
      void setStopCondition( int stopValue );

      // reset the timers and counters
      void sync( unsigned long syncTime=0L );       // start the clock

      void armPort() { _trigState = STATE::TS_ARMED; }
      void haltPort() { _trigState = STATE::TS_HALT; }

      // intended to be placed in the loop() stub to periodically check the
      // state of the analog channel. It will react to the set of the trigger
      // conditions and sample rate. It is intended to be non-blocking. It
      // will also test if stop conditions are met. Returns true if
      // data values were updated.
      // If no data is taken then this takes ~10μs on an uno
      // If data is taken then this takes ~140μs on an uno
      bool pollPort();

      // Getters for data and states
      unsigned long getAbsTime() { return _absTime; }
      int           getLastRead() { return _rawReading; }
      unsigned long getCount() { return _count; }
      unsigned long getCurrentTime();
      float         getMeasurement() { return applyCalibration(_rawReading); }
      const char*   getUnits() { return _units; } // return sensor's units
      // int           getState() { return _trigState; } // return current trigger state
      // unsigned long getRate()  { return _sampPeriod; } // return the actual sample period
      // int           getLevel() { return _trigLevel; }
      // int           getCond()  { return _trigCond; }

      String        getStatus( const char* open );


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
      unsigned long _absTime;      // time of last read ofset from sync
      unsigned long _sampPeriod;   // how long to wait between readings 0 is button push
      unsigned long _nextRead;     // time to take next reading
      unsigned long _start_us;     // marker for start sequence
      unsigned long _count;        // count of values

      unsigned long _stopCond;     // could be count or time
//      bool          _stopMethod;   // if true then time, false then count.

      int           _trigState;    // trigger state: HALT, ARMED, RUN
      int           _trigCond;     // triggerconditions: IMMEDIATE, FALL_BELOW, or RISE_ABOVE
      int           _trigLevel;    // level that can cause trigger
};

#endif
