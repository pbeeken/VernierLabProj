/***
 * Test Platform for Vernier Interface Shield
***/

#include <Streaming.h>
#include <VernierBlinker.h>

VernierBlinker theLED;

// Setup the conditions
void setup() {

  Serial.begin( 115200 );

  Serial << "Testing Blink Sensor." << endl;

  Serial << "5 blinks, default period" << endl;
  theLED.blinkFor(5);

  Serial << "20 blinks, 10Hz" << endl;
  theLED.setBlinkPeriod(100);
  theLED.blinkFor(20);

  delay(1000); // 1 second

  theLED.turnOff();

  theLED.setBlinkPeriod(2000);

}

// Global values that persist through different calls
unsigned long lastTime = 0L;

//  Loop is executed repeatedly
void loop() {
  if( millis()>100000 ) return;
  theLED.update();  // periodic checks
}
