/***
 * Test Platform for Vernier Interface Shield
***/

#include <Streaming.h>
#include <VernierBlinker.h>
#include <VernierVoltage.h>

VernierBlinker theLED;
VernierVoltage theSensor( VernierVoltage::BTA01 );

bool amIdone();

void setup() {

  Serial.begin( 115200 );

  theLED.turnOn();
  Serial << "Testing Diff Voltage." << endl;
  delay(1000); // 1 second
  theLED.turnOff();
}

unsigned long accValue = 0L;
float accReading = 0.0;
int cnt=0;
unsigned long lastTime = 0L;

void loop() {

	if ( amIdone() ) return;

  AnalogReading value = theSensor.readValue();
  cnt++;
  accReading += value.reading;
  accValue += value.adc;


  if ((millis()-lastTime)>1000) {
    // timer ran out
    theLED.turnOn();

    Serial << "r: " << (accValue/cnt) << " v:" << (accReading/cnt) << theSensor.sensorUnits()  << endl;
    cnt = 0;
    accReading=0.0;
    accValue=0;
    lastTime = millis();

    theLED.turnOff();
  }

  delay( 10 );

}


bool amIdone() {
  if ( millis() > 100000 ) {
    if (cnt != 0)
      Serial << "I'm done here." << endl;
    cnt = 0;
    return true; // after 100 seconds quit.
  }
  return false;
}
