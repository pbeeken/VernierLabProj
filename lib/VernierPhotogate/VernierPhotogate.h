/****************************************************************
VernierPhotogate
   This has been completely refactored in light of the more
   general VernierDigitalSensor object.

   Signals from the gates are in the form of digital voltages.
   +5V or true is an open gate while 0V or false for a blocked gate.
   This object uses the metaphors for HIGH=OPEN and LOW=BLOCKED

   Photogates are very versitile tools when used alone and in various
   combinations They work based on a simple principal: a beam of an IR LED is
   aimed at a photocell when the light is blocked the output signal reads
   'false' or 0, when the beam is restored the signal reads 'true' or 1.  Timing
   the transistion can give information about speed.  Counting can give
   information on displacement. To make matters even more interesting Vernier
   allows 'daisy chains' of photogates where you can connect gates in series

   Originally Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 10.11.2016

   Refactored to use the VernierDigitalSensor object
   PBeeken ByramHills High School 10.11.2016

****************************************************************/
#ifndef VernierPhotogate_h
#define VernierPhotogate_h

#include <VernierDigitalSensor.h>
#include <Arduino.h>

// Gate state conditions
enum GATES {
  OPEN = HIGH,
  BLOCKED = LOW,
};

class VernierPhotogate: public VernierDigitalSensor
{
  public:
      // 	Basic tools, reads a sensor and returns the time index along
      // return metahors for the gate states in an immediate fashion.
      bool isGateOpen() { return readPort(); };
      bool isGateBlocked() { return !readPort(); }

      // with gates there are specific combinations of timing events
      // that are used with certain arrangements of gates. These methods are
      // meant to simplify these combinations.

      // a simple 'time the race' setup.  Time the falling edge (when the racer)
      // first crosses the start to when it crosses the finish
      void timeBetweenGatesSetup();    // setup the experiment
      bool timeBetweenGatesTiming();   // put into loop() stub to determine
                                       // timing. returns true when timing done.

      // a flag (opaque block of known length) is mounted on a dynamics cart
      // with two photogates chained to a single digital input. After being
      // setup the first blocked time can be used to determine the initial
      // speed, the time between the gates can be used to determine the a
      // average speed of travel while the final time can determine the final
      // speed.  A common experiment is one where the distincion between
      // instantaneous speed vs average speed can be illustrated.
      void sequentialFlagSetup();  // call to setup this experiment
      bool sequentialFlagTiming(); // put into loop() stub to grab the times
                                   // return true if sequence is done.

	// elements for subclassing
  protected:

	// exclusive to this object
  private:
     // three sequence timing.
      unsigned long timingValues[3];
};

#endif
