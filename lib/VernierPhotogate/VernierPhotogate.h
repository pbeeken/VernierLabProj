/****************************************************************
VernierPhotogate
	This is the superclass designated to serve as the model for
	all other digital sensors. It is unlikely anyone would use this
	class directly.  The way to use this object is through the
	subclass associated with the specific sensor that needs to
	be used.

   Photogates are very versitile tools when used alone and in various
   combinations They work based on a simple principal: a beam of an IR LED is
   aimed at a photocell when the light is blocked the output signal reads
   'false' or 0, when the beam is restored the signal reads 'true' or 1.  Timing
   the transistion can give information about speed.  Counting can give
   information on displacement. To make matters even more interesting Vernier
   allows 'daisy chains' of photogates where you can connect gates in series

   All times are measured in microseconds.  Times returned by methods are either
   delta or absolute. Absoulte times are measured since the time marked by
   "resetClock".

   Signals from the gates are in the form of digital voltages.  +5V or true is
   an open gate while 0V or false for a blocked gate. There are timing tools one
   the Arduino that make these signals easy to time.  Signals can be timed using
   these tools.

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

	11 Nov 2016- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef VernierPhotogate_h
#define VernierPhotogate_h

// All readings use this object for storing analog values.
struct SequentialTimes {
  unsigned long firstPulseWidth;
  unsigned long secondPulseWidth;
  unsigned long timeBetweenGates;
};

// Gate state conditions
enum {
  CHANGES,
  BLOCKED,
  CLEARED
};

#define   u1_SEC  1000000  // usec
#define   u5_SEC  5000000  // usec
#define  u10_SEC 10000000  // usec


/****
Photogates are very versitile tools when used alone and in various combinations
They work based on a simple principal: a beam of an IR LED is aimed at a
photocell when the light is blocked the output signal reads 'false' or 0, when
the beam is restored the signal reads 'true' or 1.  Timing the transistion can
give information about speed.  Counting can give information on displacement. To
make matters even more interesting Vernier allows 'daisy chains' of photogates
where you can connect gates in series


*****/
class VernierPhotogate
{
  public:
    VernierPhotogate( int channel );			// Constructor. Mainly sets up pins.

      // 	Basic tools, reads a sensor and returns the time index along
//    DigitalReading readGate();   // read raw state of gate with timestamp
      // read the current state of the gate.  True if gate is clear
    bool isGateClear();

      // Get a sequence of times. The caller spells out how many transitions
      //   to time.  The call takes the current state (blocked or unblocked) and
      //   returns an array of n times which subsequent transitions take place.
      // @returns false if a timeout condition ensued.
    bool gatherTimes( unsigned long timeout_us, int n, unsigned long *times );

      // Get three times.  Time in first gate, time between gates and time in
      //   gate again.  Common setup for this is a daisy chained set to 2 gates
      //   with known distance between gates.
      // @return false if a timeout condition ensues.
    bool sequentialTiming( unsigned long timeout_us, SequentialTimes *times );

      // time the block (or clear) of a gate.
      //    This routine times the transition from Block to Block until
      //    object clears gate. If gate is blocked already it will wait
      //    until it clears before beginning the timer again.
    unsigned long timeInGate( int trigger, unsigned long timeout_us ); // time the passing of gate

      //  reset the timers
    void          resetClock();       // start the clock

	// constants for channels
    const static int BTD01  = 2;  // D2
    const static int BTD02  = 6;  // D6


	// elements for subclassing
  protected:

	// exclusive to this object
  private:
    int           _channel;
    unsigned long _start_us;
//    unsigned long _start_ms;

};

#endif
