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

  SequentialTimes seq;

  bool success = theFirstGate.
  sequentialTiming(u5_SEC,&seq);

   // timing a pass through the gate
   if( success ) {
     theLED.turnOn();
     Serial << " time in gateA " << seq.firstPulseWidth << " us" << endl;
     Serial << " time between gate " << seq.timeBetweenGates << " us" << endl;
     Serial << " time in gateB " << seq.secondPulseWidth << " us" << endl;
     theLED.turnOff();
  }

delay(10);

}
