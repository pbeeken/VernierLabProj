/***
 * Test Platform for Vernier Interface Shield
***/
#include <Arduino.h>
#include <Streaming.h>
#include <VernierBlinker.h>
#include <VernierDigitalSensor.h>
#include <VernierButton.h>

VernierBlinker theLED;
VernierButton  theButton;
VernierDigitalSensor aGate( VernierDigitalSensor::BTD01 );

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Photogate Operations." << endl;
  Serial << "10 blinks, 20Hz" << endl;
  theLED.setBlinkPeriod(200);
  theLED.blinkFor(10);

//  Serial << "Waiting for button push" << endl;
//  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();

  Serial << "Go..." << endl;
  aGate.setTrigger(DTRIGCOND::ANY);
  aGate.haltPort();

}



//  Loop is executed repeatedly
void loop() {
  static int modeCount = 0;

 // routine polling
 if ( aGate.pollPort() ) {
      Serial << "==== " << aGate.getCount() << (aGate.getTransitionType()==DTRIGCOND::LOW2HIGH?"R ":"F ")
             << " -a " << aGate.getAbsTime() << " (" << (aGate.getAbsTime()/1.0E6) << ")"
             << " -d " << aGate.getDeltaTime() << " (" << (aGate.getDeltaTime()/1.0E6) << ")"
             " ===" << endl;
   }

// If button hit then report state
if ( theButton.buttonIsDown() ) {
   // trick for waiting unitl slow meat lets go.
   while( theButton.buttonIsDown() );
   aGate.sync();
   aGate.armPort();
   }

if ( aGate.getCount()%10 == 9 ) {
   modeCount = ++modeCount % 3;
   Serial << endl << "====Changing Modes====" << endl;
   if( modeCount==0 )
      aGate.setTrigger(DTRIGCOND::ANY);
   if( modeCount==1 )
      aGate.setTrigger(DTRIGCOND::LOW2HIGH);
   if( modeCount==2 )
      aGate.setTrigger(DTRIGCOND::HIGH2LOW);
}





}
