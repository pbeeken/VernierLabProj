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

  Serial << "Testing Analog Timing Operations." << endl;
  Serial << "5 blinks, 5Hz" << endl;
  theLED.setBlinkPeriod(200);
  theLED.blinkFor(5);

//  Serial << "Waiting for button push" << endl;
//  while( !theButton.buttonIsDown() );  // Wait for button push

  theLED.turnOff();
  aGate.setSampleRate( SAMPLERATES::S_20Hz );  // 20 Hz
  aGate.setStopCondition( false, 10 ); // put out 10 values
  aGate.setTrigger( TRIGCOND::TS_IMMEDIATE, 0);

  Serial << "Go..." << endl;

}

int modeCount = 0;

//  Loop is executed repeatedly
void loop() {

  if ( aGate.pollPort() ) { // handles everything given the stating point
         Serial << "==== " <<
            aGate.getCount() << " " <<
            aGate.getAbsTime() << " " <<
            aGate.getLastRead() << " " <<
            aGate.getUnits() << " ====" << endl;
      }

   // If button hit then report state
   if ( theButton.buttonIsDown() ) {
      // trick for waiting unitl slow meat lets go.
      while( theButton.buttonIsDown() );
        aGate.sync();  // reset clock first as this disables triggering
        aGate.armPort();
      }
}
