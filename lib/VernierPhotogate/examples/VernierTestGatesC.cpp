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


//  Loop is executed repeatedly
void loop() {

  unsigned long seq[5];

  bool success = theFirstGate.gatherTimes(u5_SEC,5,seq);

   // timing a pass through the gate
   if( success ) {
     theLED.turnOn();
     for( int j=0;j<5;j++ ) 
        Serial << " time["<<j<<"]" << seq[j] << " us" << endl;
     Serial << "  =====++=====" << endl;
     theLED.turnOff();
  }

delay(10);

}
