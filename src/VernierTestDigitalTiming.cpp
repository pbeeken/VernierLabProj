/***
 * Test Platform for Vernier Interface Shield
***/
#include <Arduino.h>
#include <Streaming.h>
#include <VernierBlinker.h>
#include <VernierDigitalInput.h>
#include <VernierButton.h>

VernierBlinker theLED;
VernierButton  theButton;
VernierDigitalInput aGate( VernierDigitalInput::BTD01 );

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Photogate Operations." << endl;
  Serial << "10 blinks, 10Hz" << endl;
  theLED.setBlinkPeriod(100);
  theLED.blinkFor(10);

//  Serial << "Waiting for button push" << endl;
//  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();

  aGate.initTrigger(HIGH2LOW);

  Serial << "Go..." << endl;

}

//  Loop is executed repeatedly
void loop() {

 if ( aGate.pollPort() )
      Serial << aGate.getTransitions() << " - " << aGate.getDeltaTime() << endl;

if ( theButton.buttonIsDown() ) {
   // trick for waiting unitl slow meat lets go.
   while( theButton.buttonIsDown() );
   Serial << " S: " << aGate.readPort() << endl;
   }
}
