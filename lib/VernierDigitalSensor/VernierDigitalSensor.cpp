/****************************************************************
VernierDigitalSensor
	This is the superclass designated to serve as the model for
	all other digital sensors.
              <--RISING_T-->
                       <--FALLING_T-->
             +-------+       +-------+
             |       |       |       |
             |       |       |       |
      +------+       +-------+       +-------+
         <-ANY-> <-ANY-> <-ANY->

Tested and developed in Platformio 3.1.0

Signals from the gates are in the form of digital voltages.  +5V or true is
an open gate while 0V or false for a blocked gate. There are timing tools one
the Arduino that make these signals easy to time.  Signals can be timed using
these tools.

Tested and developed in Platformio 3.1.0
PBeeken ByramHills High School 7.21.2017

****************************************************************/
#include <Arduino.h>
#include <VernierDigitalSensor.h>


/** Constructor
 *    setup button channel and initialize vars.
 */
VernierDigitalSensor::VernierDigitalSensor( int channel ) {
  _channel = channel;
  begin();
  // default is to time how long the gate is in a state.
  _trigger = ANY;
  _lastState = UNDETERMINED;
}

/** setTrigger
 *    Set trigger state and initialize the system for timing. N.B. this
 *    routine will hold for the proper transition for the required state
 *    so the data 'pump is primed'.
 */
void
VernierDigitalSensor::setTrigger( int trigger ) {
   _trigger = trigger;
   // we need to hold to wait for right condition to start.
   switch( _trigger ) {
      case ANY:
               _lastState = readPort(); // store current state.
               break;
      case FALLING_T:
               _lastState = LOW;
               while (_lastState==LOW) { // wait for signal to go HIGH
                     _lastState = readPort(); // store current state.
                  }
               break;
      case RISING_T:
               _lastState = HIGH;
               while (_lastState==HIGH) { // wait for signal to go LOW
                  _lastState = readPort(); // store current state.
               } // exit when _lastState is LOW.
               break;
   }
   begin();
}

/** pollGate
 *    intended to be put into loop to read transistion of signals
 *    This routine assumes it is being called regularly in the
 *    loop() stub.  A better tool would be to run an ISR but I
 *    don't think this is available to us.
 */
bool
VernierDigitalSensor::pollPort() {
   // get the current state and time.
   bool currState = readPort();
   unsigned long currTime = micros();

   // has the state hasn't changed do nothing report back.
   if ( currState == _lastState ) return false;

   switch( _trigger ) {
      case ANY:    // Any change of state.
               _lastState = currState;
               break;

      case RISING_T:  // looking for a falling trigger
               if ( currState == LOW && _lastState == HIGH ) {
                  _lastState=currState;
                  return false;
                  }
               break;

      case FALLING_T:    // looking for a rising trigger
               if ( currState == HIGH && _lastState == LOW ) {
                  _lastState=currState;
                  return false;
                  }
               break;
   }

   // satisfied trigger
   // record state and flag that we triggered.
   _transitionCount++; // count transitions
   _lastState = currState;
   _transitionType = currState == HIGH ? RISING_T : FALLING_T;
   _deltaTime = (unsigned long)currTime - _absTime;
   _absTime = currTime; // reset timer (we only report Deltas)
   return true; // successful detection of trigger condition.
}

/** read the current state of the gate
 *    true is an open gate, false is a blocked gate
 */
bool
VernierDigitalSensor::readPort() {
  return digitalRead(_channel);
}

/** reset the timer starting now!
 *    resets the internal values for subtracting from any subsequent times
 *    somewhere in the future maybe this can be used to reset the universal
 *    clock.
 */
void
VernierDigitalSensor::begin() {
   _transitionType = UNDETERMINED;
   _transitionCount = 0L;
   _deltaTime=0L;
   _absTime=0L;
   _start_us=micros();
}
