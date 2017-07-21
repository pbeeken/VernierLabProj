/***
 * Test Platform for Vernier Interface Shield
***/
#include <Arduino.h>
#include <Streaming.h>
#include <VernierBlinker.h>
#include <VernierPhotogate.h>

#include <VernierButton.h>

VernierBlinker theLED;
VernierButton  theButton;
VernierPhotogate theFirstGate( VernierPhotogate::BTD01 );
VernierPhotogate theSecondGate( VernierPhotogate::BTD02 );

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Photogate Operations." << endl;
  Serial << "20 blinks, 10Hz" << endl;
  theLED.setBlinkPeriod(100);
  theLED.blinkFor(20);

  Serial << "Waiting for button push" << endl;
  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();
  Serial << "Go..." << endl;

//  theLED.setBlinkPeriod(2000);

}

// Global values that persist through different calls
unsigned long lastTime = 0L;

//  Loop is executed repeatedly
void loop() {


// timing example within a single gate
   unsigned long tgm = 0L;
   unsigned long tgn = 0L;

  // Example of manual timing
  if( theFirstGate.waitForGate(BLOCKED, u5_SEC ) ) {
    unsigned long beg = micros();  // get the time
    if ( theFirstGate.waitForGate(CLEARED, u1_SEC) ) {
      tgm = micros()-beg;  // record the difference
    }
    else
      Serial << " **timout cleared ** " << endl;
  }
  else
    Serial << "  **timeout waiting**" << endl;

// automatic timing
   tgn = theFirstGate.timeInGate( BLOCKED, u1_SEC );

// comparison of results: manual mode is almost always slower.
   if ( tgm > 0 && tgn == 0 ) {
     Serial << " MM through: " << tgm  << endl;
   }
   else if ( tgn > 0 && tgm == 0 ) {
     Serial << " AM through: " << tgn  << endl;
   }
   else if ( tgn > 0 && tgm >> 0 ) {
     Serial << " MM through: " << tgm << " AM through: " << tgn << " Del: " << (long)tgm-(long)tgn << endl;
     double v1 = 12.5 * 1000 / tgm;
     double v2 = 12.5 * 1000 / tgn;
     Serial << " vm " << v1 << " va " << v2 << " del " << v2-v1 << endl;

}


delay(100);

}
