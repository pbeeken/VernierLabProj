/***
 * Test Platform for Vernier Interface Shield
***/

#include <Streaming.h>
#include <Vernier1DAccelerometer.h>
#include <VernierBlinker.h>

VernierBlinker theLED;
//VernierDetect theSensor( VernierDetect::BTA01 );
Vernier1DAccelerometer theSensor( Vernier1DAccelerometer::BTA01 );
bool IamDone();

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  theLED.turnOn();
  Serial << "Testing Accelerometer." << endl;
  delay(1000); // 1 second
  theLED.turnOff();

}

// Global values that persist through different calls
unsigned long accValue = 0L;
float accReading = 0.0;
int cnt=0;
unsigned long lastTime = 0L;

//  Loop is executed repeatedly
void loop() {

  if ( IamDone() ) {
    return; // after 100 seconds quit.
    }

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

// kills the job after 100s
bool IamDone() {
  if ( millis() > 100000 ) {
    if (cnt != 0)
      Serial << "I'm done here." << endl;
    cnt = 0;
    return true;
  }
  return false;
}
