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

  aGate.initTrigger(ANY);

  Serial << "Go..." << endl;

}

int modeCount = 0;

//  Loop is executed repeatedly
void loop() {

 // routine polling
 if ( aGate.pollPort() ) {
      Serial << aGate.getTransitionCount() << " ";
      Serial << (aGate.getTransitionType()==RISING_T?"R ":"F ");
      Serial << aGate.getDeltaTime() << endl;
   }

// If button hit then report state
if ( theButton.buttonIsDown() ) {
   // trick for waiting unitl slow meat lets go.
   while( theButton.buttonIsDown() );
   Serial << " S: " << aGate.readPort() << endl;
   }

if ( aGate.getTransitionCount()%50 == 49 ) {
   modeCount = ++modeCount % 3;
   Serial << endl << "====Changing Modes====" << endl;
   if( modeCount==0 )
      aGate.initTrigger(ANY);
   if( modeCount==1 )
      aGate.initTrigger(RISING_T);
   if( modeCount==2 )
      aGate.initTrigger(FALLING_T);
}





}
