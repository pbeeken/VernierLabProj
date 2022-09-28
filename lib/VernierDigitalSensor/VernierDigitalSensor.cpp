/****************************************************************
   VernierDigitalSensor
   This is the superclass designated to serve as the model for
   all other digital sensors.
        <-- LOWTOHIGH -->
                 <-- HIGHTOLOW-->
        +-------+       +-------+
        |       |       |       |
        |       |       |       |
|+------+       +-------+       +-------+
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
// #include <Streaming.h>  // DEBUG


/** Constructor
 *    setup button channel and initialize vars.
 */
VernierDigitalSensor::VernierDigitalSensor( int channel ) {
        _channel = channel;
        pinMode(_channel, INPUT);

        // default is to time how long the gate is in a state.
        setTrigger( DTRIGCOND::ANY ); // also syncs clock.
}

/** pollGate
 *    intended to be put into loop to read transistion of signals
 *    This routine assumes it is being called regularly in the
 *    loop() stub.  This is non-blocking. It returns true if a trigger
 *    condition illicited a data update.  Unlike analog ports digital
 *    gates are either armed or halted.  Total trip through here can
 *    be from 8-12μS
 */
bool
VernierDigitalSensor::pollPort() {

        // unsigned long dbg_st = micros(); // DEBUG

        if ( _trigState==0 ) return false;  // if we are halted we leave

        //  Serial << "    1:" << ( micros() - dbg_st ) << endl; // DEBUG  <4μS
        //                dbg_st = micros();                     // DEBUG

        // get the current state and time.  ~10μS
        bool currState = readPort();
        unsigned long currTime = (unsigned long)micros()-_start_us;  // time relative to sync

        // Serial << "    2:" << ( micros() - dbg_st ) << endl; // DEBUG  <12μS
        //               dbg_st = micros();                     // DEBUG

        // has the state hasn't changed do nothing.
        if ( currState == _lastState ) return false;

        switch( _trigger ) { // decisions based on the _trigger we are looking for.
        case DTRIGCOND::ANY:  // Any change of state.
                _lastState = currState; // record the read and get data
                break;

        case DTRIGCOND::LOW2HIGH: // looking for a rising trigger
                if ( currState == LOW && _lastState == HIGH ) {
                        _lastState=currState; // we had a drop, do nothing
                        return false;
                }
                break;

        case DTRIGCOND::HIGH2LOW: // looking for a falling trigger
                if ( currState == HIGH && _lastState == LOW ) {
                        _lastState=currState; // we had a rise, do nothing
                        return false;
                }
                break;
        }

        // satisfied trigger (n.b. we are here because something changed)
        // record state and flag that we triggered.
        _transitionCount++; // count transitions
        _lastState = currState;
        _transitionType = currState == HIGH ? DTRIGCOND::LOW2HIGH : DTRIGCOND::HIGH2LOW;
        _deltaTime = (unsigned long)currTime - _absTime; // time since last read
        _absTime = (unsigned long)currTime; // record the time since last sync

        // Serial << "    3:" << ( micros() - dbg_st ) << endl; // DEBUG  <8μS
        //               dbg_st = micros();                     // DEBUG

        return true; // successful detection of trigger condition.
}

/** read the current state of the gate
 *    true is an open gate, false is a blocked gate
 *    we don't record time because when we ask for the state we are not
 *    timing only checking for condition.
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
VernierDigitalSensor::sync( unsigned long syncTime ) {
        _start_us = syncTime > 0 ? syncTime : micros();
        _transitionCount = 0L;
        _transitionType = DTRIGCOND::UNDETERMINED;
        _deltaTime=0L;
        _absTime=0L;
}

/** setTrigger
 *    Set trigger state and initialize the system for timing. N.B. this
 *    routine will hold for the proper transition for the required state
 *    so the data 'pump is primed'.
 */
void
VernierDigitalSensor::setTrigger( int trigger ) {
        haltPort();
        _trigger = trigger;
        _transitionType = DTRIGCOND::UNDETERMINED;
        _lastState = readPort(); // store current state.
        // we changed the state so reinitilize everything.
        sync();
}

/** getCurrentTime
 *    An immediate command that returns the current time since last sync
 */
unsigned long
VernierDigitalSensor::getCurrentTime() {
        return micros() - _start_us;
}

/**
 *
 */
String
VernierDigitalSensor::getStatus( const char* open ) {
  String msg(open);
  msg += _trigState==0 ? "ste: H " : "ste: R ";
  switch( _trigger ) {
    case DTRIGCOND::UNDETERMINED:   msg += "con: U "; break;
    case DTRIGCOND::HIGH2LOW:       msg += "con: ⇘ "; break;
    case DTRIGCOND::LOW2HIGH:       msg += "con: ⇗ "; break;
    case DTRIGCOND::ANY:            msg += "con: A "; break;
  }

  return msg;
}
