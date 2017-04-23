/****************************************************************
VernierBlinker
	This is a class designated to control the LED on the
	SparkFun Vernier Shield.  The blue LED can be used for a
	variety of user interface feedback signals.
  Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016
****************************************************************/

#include <Arduino.h>
#include <VernierBlinker.h>

/** Constructor
 *	Sets the pin for the LED
 **/
VernierBlinker::VernierBlinker(int ledPin) {
  _ledPin = ledPin;
  pinMode(_ledPin, OUTPUT);
  _active = false;
  _blinkPeriod = 500;
  _change = millis() + _blinkPeriod;
  _ledIsOn = false;

}

/** update is to be placed in the loop() to follow through with the status changes
 *
 **/
void
VernierBlinker::update() {
	if ( _active )
		if ( millis() > _change ) {
			_ledIsOn = !_ledIsOn;
			digitalWrite( _ledPin, _ledIsOn );
			_change = millis() + _blinkPeriod;
		}
}

/*** setBlinkPeriod changes the blinking period (the time for the on and off semaphore)
 *	we could introduce a duty cycle parameter as well that will change the ratio of on
 *	to off period but that can be for another day.
 **/
void
VernierBlinker::setBlinkPeriod( unsigned long period ) {
	_blinkPeriod = period/2;
	_active = true;
  	_change = millis() + _blinkPeriod;
}

/** blinkFor is a blocking routine that blinks for a determined number of times.
 *	This can be great for giving feed back for a particular state of the robot.
 **/
void VernierBlinker::blinkFor( int nTimes ) {
	while( nTimes-- ) {
		digitalWrite( _ledPin, HIGH );
		delay( _blinkPeriod );
		digitalWrite( _ledPin, LOW );
		delay( _blinkPeriod );
	}
}

/** turnOn turns the LED on
 **/
void VernierBlinker::turnOn() {
   digitalWrite( _ledPin, HIGH );
 }

 /** turnOn turns the LED on
  **/
void VernierBlinker::turnOff() {
    digitalWrite( _ledPin, LOW );
  }
