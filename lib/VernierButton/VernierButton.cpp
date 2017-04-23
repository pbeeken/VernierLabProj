/****************************************************************
VernierButton
	This is a class designated to read the button on the
	SparkFun Vernier Shield.  The button can be used for a
	variety of user interface controls.

Tested and developed in Platformio 3.1.0
PBeeken ByramHills High School 9.1.2016

Code extensively modified to debounce the button push and provide
some utilities to detect multiple pushes since this is our only input mode

17 Aug 2015- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierButton.h>

// Maximum time to wait to identify a definite down push of a button
#define RELEASE_WAIT_TIME_MS 50
// human reaction time is, on average, about 200ms.  300 we'll wait 50% more
// useful for multiple button taps
#define PUSH_WAIT_MS	300

#ifdef USEINTERUPTS
// ISR Routine
volatile int VernierButton::buttonCount = 0;
void
VernierButton::buttonISR() {
  VernierButton::buttonCount++;
}
#endif

/** Constructor
 *    setup button read.
 */
VernierButton::VernierButton( int pin )
{
  // Sets the "default" state of the button to be HIGH.
  pinMode(pin, INPUT_PULLUP);
  _buttonPin = pin;

}

/** readSinglePush detects a button down and waits (for a short while) for the release
 * 					call doesn't block for more than RELEASE_WAIT_TIME_MS in ms.
 * @return {boolean} whether the button was released in the requisite time
 */
bool VernierButton::readSinglePush()
{
 	if ( buttonIsDown() ) { // button is down
 		// We might have a button push.
 		delay( 2 ); // wait for it... (electrical debounce)
 		if ( buttonIsDown() ) { 	// still down...
 			unsigned long stop = millis() + RELEASE_WAIT_TIME_MS;
 			while( buttonIsDown() && (millis() < stop) ); 	// wait for release but not more than RELEASE_WAIT_TIME_MS
 			return !buttonIsDown(); 	// return the state of the button now in case we timed out above.
 		}
 	}
   return false;
}

/** readDoublePush detects a double button tap.  Use this as a model to detect more.
 * This routine has to block for at least PUSH_WAIT_MS
 * @return {boolean} whether the button successfully generated a double tap.
 */
boolean VernierButton::readDoublePush()
{
 	return readPushes() == 2;
}

/** readPushes counts pushes of the button by waiting for successive releases within a time interval
 * NOTE: trying to take in mulitple hits gets tricky after a double tap.  Some people can't get the
 *        rapid rhythm of the taps to make it work past 3.  Heck, even double taps are tough.
 * @return {int} the count of button releases (taps)
 */
int VernierButton::readPushes()
{
#ifdef USEINTERUPTS
  int bp = buttonCount;
  buttonCount = 0;
  return bp;
#else
	int pushCount = 0;
	unsigned long stop;

  if ( !buttonIsDown() ) return 0; // If button is not already down, don't block

	stop = millis()+PUSH_WAIT_MS;  	// set the stop time
	while ( millis() < stop ) {  	// as long as we haven't reached the end
		if ( readSinglePush() ) { 	// we got a push within the time zone
			++pushCount;
			stop = millis()+PUSH_WAIT_MS;  // reset the clock
			}
		}
	return pushCount;
#endif
}

/** Raw button read state.  A pushed button returns true, while a un-pushed button is false
 * @return {boolean} the current button state
 */
bool VernierButton::buttonIsDown() {
	return !digitalRead(_buttonPin);  // value is pulled high until it is pressed
}
