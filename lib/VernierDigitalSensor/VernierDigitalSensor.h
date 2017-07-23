/****************************************************************
VernierDigitalInput
	This is the superclass designated to serve as the model for
	all other digital sensors. The way to use this object is through the
	subclass associated with the specific sensor that needs to
	be used.

   All times are measured in microseconds.  Times returned by methods are
   based on the resetClock method which sets the zero point of measurement.
   The curious thing about unsigned arithmetic subraction is that overflow
   is handled automatically. Although micros() wraps about every 50min it
   is unlikely we are going to need to use digital timing to this long level
   if it should prove necessary we can build a similar object or modify this
   one to use millis().

   Digital ports have two states: HIGH[+5V] or LOW[0] but we time on
   transitions: HIGH2HIGH, LOW2LOW, HIGH2LOW, LOW2HIGH, or any CHANGES
   [keywords]. This means that when we change trigger modes we need to
   'prime the pump' so that the state is correct before timing can start.
   Keep this in mind as you set up the object for measurement.

   Signals from the gates are in the form of digital voltages.  +5V or true is
   an open gate while 0V or false for a blocked gate. There are timing tools one
   the Arduino that make these signals easy to time.  Signals are timed using
   the Arduino micros() function which has a practical 4us resolution. Depending
   on the surrounding code timing bandwidths of greater than 50kHz may be
   impractical.
            <--RISING_T-->
                    <--FALLING_T-->
           +-------+       +-------+
           |       |       |       |
           |       |       |       |
    +------+       +-------+       +-------+
            <-ANY-> <-ANY-> <-ANY->

	Tested and developed in Platformio 3.1.0

   This first iteration only worries about timing transistions and which
   direction they are going in.  We'll leave more sophisticated modes to
   subclasses.
	PBeeken ByramHills High School 9.1.2016

   21 Aug 2017- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef VernierDigitalSensor_h
#define VernierDigitalSensor_h


// Digital trigger conditions
enum TRIGSTATES {
  UNDETERMINED,
  LOW2HIGH,
  HIGH2LOW,
  ANY,
  RISING_T,
  FALLING_T,
};

class VernierDigitalSensor
{
  public:
   	// Constructor. Mainly sets up pins.
      VernierDigitalSensor( int channel );

      // read the current digital level.
      // True if level is high, false if low.
      bool readPort();

      // intended to be placed in the loop() stub to periodically check the
      // state of the digital channel. It will react to the set of the trigger
      // condition. Returns true if a trigger condition was met.
      bool pollPort();

      // Set trigger state and initialize the system for timing. N.B. this
      // routine will hold for the proper transition for the required state.
      void initTrigger( int trigger );

      // reset the timers and counters
      void begin();       // start the clock

      // get the last delta time
      unsigned long getDeltaTime() { return _deltaTime; }
      unsigned long getAbsTime() { return _absTime; }
      unsigned long getTransitionCount() { return _transitionCount; }
      char getTransitionType() { return _transitionType; }

      // constants for channels
      const static int BTD01  = 2;  // D2
      const static int BTD02  = 6;  // D6

      // elements for subclassing
  protected:

	   // exclusive to this object
  private:
      int           _channel;          // digital channel to read from
      int           _trigger;          // condition for timing to take place
      char          _transitionType;   // slope of last transition
      bool          _lastState;        // last state of digital read [for determining slope]
      unsigned long _deltaTime;        // time since last trigger condition
      unsigned long _transitionCount;  // absolute count of trigger conditions
      unsigned long _absTime;          // absolute timing of trigger relative to start
      unsigned long _start_us;         // mark the start time

};

#endif
