/****************************************************************
VernierBlinker
	This is a class designated to control the LED on the
	SparkFun Vernier Shield.  The blue LED can be used for a
	variety of user interface feedback signals.
	PBeeken ByramHills High School 9.1.2016
****************************************************************/

#ifndef VernierBlinker_h
#define VernierBlinker_h

class VernierBlinker
{
	public:
		VernierBlinker( int pin=13 );	// Constructor with led connected to pin.

		void update();				// placed in loop() to periodically update actions
		void setBlinkPeriod( unsigned long period=500 );	// set the blinking period
		void blinkFor( int nTimes );	// blink the led for a finite number (blocks)
		void activate() { _active = true; }		// turn on asynchronous routines
		void deactivate() { _active = false; }	// turn off asynchronous routines
		void turnOn();
		void turnOff();

	private:
		bool _active;
		unsigned long _change;
		unsigned long _blinkPeriod;
		bool _ledIsOn;
		int _ledPin;
};

#endif
