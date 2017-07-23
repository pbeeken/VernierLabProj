/****************************************************************
VernierAnalogSensor
   This is the superclass designated to serve as the model for
   all other analog sensors. The best way to use this object is
   through the subclass associated with the specific sensor or
   experiment.

   This class was refactored to better match how we read the digital
   ports.

   Tested and developed in Platformio 3.1.0
   PBeeken ByramHills High School 9.1.2016

   17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierAnalogSensor.h>

/** Constructor
 *    setup channels and initialize values.
 */
VernierAnalogSensor::VernierAnalogSensor( int channel )
{
   static const char sUnits[] = "raw";
  _channel = channel;
  _slope    =  1.0;
  _intcpt   =  0.0;
  _units    = sUnits;
  begin();  // initialize timer.
}

/**
 * raw immediate read of selected analog channel
 */
int
VernierAnalogSensor::readPort() {
   return analogRead(_channel);
}


/**
 * raw immediate read of selected analog channel
 */
bool
VernierAnalogSensor::pollPort() {
   if ( _trigTime==0L ) { // act on a button press.
      // If button hit then report state
      if ( _btn.buttonIsDown() ) {
         // trick for waiting unitl slow meat lets go.
         while( _btn.buttonIsDown() );
         _rawReading = readPort();
         _absTime  = micros()-_start_us;
         _nextRead = micros()+_trigTime;
         return true;
         }
   } else if ( micros()>_nextRead ) {
      _rawReading = readPort();
      _absTime  = micros()-_start_us;
      _nextRead = micros()+_trigTime;
      return true;
   }
   return false;
}

/**
 * set sample time.  A small accident revealed the fastest time the Arduino
 * can sample is 1528us (~1.5ms) per call.
 */
void
VernierAnalogSensor::initTimer( unsigned long delTimeMS ) {
   _trigTime = delTimeMS*1000;
   _start_us = micros();
   _nextRead = micros()+_trigTime;
}

/** applyCalibration() is a virtual function which, by default,
 *                     applies a linear correction to the raw
 *                     digital input to generate a more meaningful
 *                     value.  This is a virtual function.
 *
 * @param {adcValue} input int from ADC.
 */
float
VernierAnalogSensor::applyCalibration( int adcValue ) {
  return _slope * adcValue + _intcpt;
}

// /** readSensor() returns a single value from the chosen analog sensor
//  *               but applies a conversion to the ADC value
//  *
//  * @return {AnalogReading} the voltage on a single sensor
//  */
// AnalogReading
// VernierAnalogSensor::readValue()
// {
//   AnalogReading rc = readSensor();
//   rc.reading = applyCalibration( rc.adc );
//
//   return rc;
// }

/** begin() reset the timer starting now!
 *
 */
void
VernierAnalogSensor::begin() {
  _start_us = micros();
  _rawReading = 0;
  _nextRead = micros()+_trigTime;
}
