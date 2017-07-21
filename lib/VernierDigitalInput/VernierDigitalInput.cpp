/****************************************************************
VernierDigitalInput
	This is the superclass designated to serve as the model for
	all other digital sensors.
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
#include <Arduino.h>
#include <VernierDigitalInput.h>


/** Constructor
 *    setup button channel and initialize vars.
 */
VernierDigitalInput::VernierDigitalInput( int channel ) {
  _channel = channel;
  resetClock();
  // default is to time how long the gate is in a state.
  _trigger = CHANGES;
  _lastState = HIGH;
}

/** setTrigger
 *    Set trigger state and initialize the system for timing. N.B. this
 *    routine will hold for the proper transition for the required state
 *    so the data 'pump is primed'.
 */
void
VernierDigitalInput::initTrigger( int trigger ) {
   _trigger = trigger;
   // we need to hold to wait for right condition to start.
   switch( _trigger ) {
      case HIGH2HIGH:
      case HIGH2LOW:
               _lastState =  HIGH;
               while (_lastState) { // wait for gate to open
                  _lastState = readPort(); // store current state.
               }
               break;
      case LOW2LOW:
      case LOW2HIGH:
               _lastState =  true;
               while (!_lastState) { // wait for gate to close
                  _lastState = readPort(); // store current state.
               }
               break;
      case CHANGES:
            _lastState = readPort(); // store current state.
   }
   resetClock();
}

/** pollGate
 *    intended to be put into loop to read transistion of signals
 *    This routine assumes it is being called regularly in the
 *    loop() stub.  A better tool would be to run an ISR but I
 *    don't think this is available to us.
 */
bool
VernierDigitalInput::pollPort() {
   // get the current state and time.
   bool currState = readPort();
   unsigned long currTime = micros();

   // has the state hasn't changed do nothing report back.
   if ( currState == _lastState ) return false;
   _transitionCount++; // count transitions

   switch( _trigger ) {
      case CHANGES:    // Any change of state.
               _lastState = currState;
               break;

      case HIGH2LOW:  // A falling trigger transition
               if ( currState == LOW && _lastState == HIGH )
                  return false;
      case LOW2HIGH:    // A rising trigger
               if ( currState == HIGH && _lastState == LOW )
                  return false;
               break;
      case LOW2LOW:
      case HIGH2HIGH:
               if ( _transitionCount%2 )
                  return false;
               break;
   }

   // satisfied trigger
   // record state and flag that we triggered.
   _timeStamp = (unsigned long)currTime - _start_us;
   _start_us = currTime; // reset timer (we only report Deltas)
   return true; // no successful detection of trigger condition.
}

/** read the current state of the gate
 *    true is an open gate, false is a blocked gate
 */
bool
VernierDigitalInput::readPort() {
  return digitalRead(_channel);
}

/** reset the timer starting now!
 *    resets the internal values for subtracting from any subsequent times
 *    somewhere in the future maybe this can be used to reset the universal
 *    clock.
 */
void
VernierDigitalInput::resetClock() {
   _transitionCount = 0L;
   _start_us=micros();
}
