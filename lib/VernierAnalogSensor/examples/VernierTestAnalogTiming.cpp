/***
 * Test Platform for Vernier Interface Shield
***/
#include <Arduino.h>
#include <Streaming.h>
#include <VernierBlinker.h>
#include <VernierAnalogSensor.h>
#include <VernierButton.h>

bool IamDone();

VernierBlinker theLED;
VernierButton  theButton;
VernierAnalogSensor aGate( VernierAnalogSensor::BTA01_5V );

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Analog Operations." << endl;
  Serial << "10 blinks, 10Hz" << endl;
  theLED.setBlinkPeriod(100);
  theLED.blinkFor(10);

//  Serial << "Waiting for button push" << endl;
//  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();
  aGate.initTimer(50000);  // 20 Hz

  Serial << "Go..." << endl;

}

int modeCount = 0;

//  Loop is executed repeatedly
void loop() {

   if ( IamDone() ) {
     return; // after 100 seconds quit.
     }

   // routine polling
   if( modeCount>0 ) {
      if ( aGate.pollPort() ) {
         Serial << aGate.getTime() << " ";
         Serial << aGate.getLastRead() << " ";
         Serial << aGate.getUnits() << endl;
      }
   }

   // If button hit then report state
   if ( theButton.buttonIsDown() ) {
      // trick for waiting unitl slow meat lets go.
      while( theButton.buttonIsDown() );
      Serial << " S: " << aGate.readPort() << endl;
      modeCount++;
      aGate.begin();
      }
}


// kills the job after 100s
bool IamDone() {
  static bool once=true;
  if ( millis() > 20000 ) {
    if ( once )  // print one time.
      Serial << "I'm done here." << endl;
    once = false;
    return true;
  }
  return false;
}
