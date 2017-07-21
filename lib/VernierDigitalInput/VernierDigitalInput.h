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
            <--   H2H   -->
                    <--  L2L    -->
           +-------+       +-------+
           |       |       |       |
           |       |       |       |
    +------+       +-------+       +-------+
            <-H2L-> <-L2H->
            <-CHG-> <-CHG-> <-CHG->

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

   21 Aug 2017- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef VernierDigitalInput_h
#define VernierDigitalInput_h


// Digital trigger conditions
enum TRIGSTATES {
  HIGH2HIGH,
  HIGH2LOW,
  LOW2HIGH,
  LOW2LOW,
  CHANGES,
};

/****
Photogates are very versitile tools when used alone and in various combinations
They work based on a simple principal: a beam of an IR LED is aimed at a
photocell when the light is blocked the output signal reads 'false' or 0, when
the beam is restored the signal reads 'true' or 1.  Timing the transistion can
give information about speed.  Counting can give information on displacement. To
make matters even more interesting Vernier allows 'daisy chains' of photogates
where you can connect gates in series
*****/

class VernierDigitalInput
{
  public:
    VernierDigitalInput( int channel );			// Constructor. Mainly sets up pins.

      // read the current state of the gate.
      // True if gate is clear, false if blocked.
    bool readPort();

      // intended to be placed in the loop() stub to periodically check the
      // state of the digital channel. It will react to the set of the trigger
      // condition. Returns true if a trigger condition was met.
    bool pollPort();

      // Set trigger state and initialize the system for timing. N.B. this
      // routine will hold for the proper transition for the required state.
   void initTrigger( int trigger );

      // reset the timers
    void          resetClock();       // start the clock

      // get the last delta time
    unsigned long getDeltaTime() { return _timeStamp; }
    unsigned long getTransitions() { return _transitionCount; }

	// constants for channels
    const static int BTD01  = 2;  // D2
    const static int BTD02  = 6;  // D6

	// elements for subclassing
  protected:

	// exclusive to this object
  private:
     int           _trigger;
     int           _channel;
     bool          _lastState;
     unsigned long _transitionCount;
     unsigned long _timeStamp;
     unsigned long _start_us;

};

#endif
