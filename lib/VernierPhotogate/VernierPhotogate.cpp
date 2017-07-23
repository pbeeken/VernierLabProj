/****************************************************************
VernierPhotogate
	This has been completely refactored in light of the more
   general VernierDigitalSensor object.

   Signals from the gates are in the form of digital voltages.
   +5V or true is an open gate while 0V or false for a blocked gate.
   This object uses the metaphors for HIGH=OPEN and LOW=BLOCKED

	Originally Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 10.11.2016

   Refactored to use the VernierDigitalSensor object
   PBeeken ByramHills High School 10.11.2016

****************************************************************/
#include <Arduino.h>
#include <VernierPhotogate.h>

// a simple 'time the race' setup.  Time the falling edge (when the racer)
// first crosses the start to when it crosses the finish
void
VernierPhotogate::timeBetweenGatesSetup() {

}    // setup the experiment

bool
VernierPhotogate::timeBetweenGatesTiming(){
   return false;
}   // put into loop() stub to determine
   // timing. returns true when timing done.

// a flag (opaque block of known length) is mounted on a dynamics cart
// with two photogates chained to a single digital input. After being
// setup the first blocked time can be used to determine the initial
// speed, the time between the gates can be used to determine the a
// average speed of travel while the final time can determine the final
// speed.  A common experiment is one where the distincion between
// instantaneous speed vs average speed can be illustrated.
void
VernierPhotogate::sequentialFlagSetup(){

}  // call to setup this experiment

bool
VernierPhotogate::sequentialFlagTiming(){
   return false;
} // put into loop() stub to grab the times
                             // return true if sequence is done.
