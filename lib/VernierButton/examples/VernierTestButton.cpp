/***
 * Test Platform for Vernier Interface Shield
***/

#include <Streaming.h>
#include <VernierBlinker.h>


#include <VernierButton.h>
#ifdef USEINTERUPTS
#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#endif
VernierBlinker theLED;
VernierButton  theButton;


// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Button Feedback." << endl;

  Serial << "20 blinks, 10Hz" << endl;
  theLED.setBlinkPeriod(100);
  theLED.blinkFor(20);

  delay(1000); // 1 second

  Serial << "Waiting for button push" << endl;
  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();
  theLED.setBlinkPeriod(2000);

#ifdef USEINTERUPTS
  // This needs to take place here.  Attempts to put this in a library failed.
  enableInterrupt( 12, theButton.buttonISR, RISING );
#endif
}

// Global values that persist through different calls
unsigned long lastTime = 0L;

//  Loop is executed repeatedly
void loop() {

#ifdef USEINTERUPTS
  if( millis()>100000 ) return;
  if ( millis() < lastTime+1000 ) {
    theLED.update();  // periodic checks
    return;  // we are done here.
    }
#else

#endif



  int bp = theButton.readPushes();
  switch( bp ) {
    case 1: Serial << "1 push" << endl;
            break;
    case 2: Serial << "2 push" << endl;
            break;
    case 3: Serial << "3 push" << endl;
            break;
    default:
            if ( bp > 0 )
              Serial << bp << " push" << endl;
            break;
  }

  theLED.update();  // periodic checks
  lastTime = millis();

}
