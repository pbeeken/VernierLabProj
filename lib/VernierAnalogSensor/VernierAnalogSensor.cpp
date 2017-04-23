/****************************************************************
VernierAnalogSensor
	This is the superclass designated to serve as the model for
	all other analog sensors. It is unlikely anyone would use this
	class directly.  The way to use this object is through the
	subclass associated with the specific sensor that needs to
	be used.

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

	17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierAnalogSensor.h>

/** Constructor
 *    setup channls and initialize values.
 */
VernierAnalogSensor::VernierAnalogSensor( int channel )
{
   static const char sUnits[] = "none";
  _channel = channel;
  _slope    =  1.0;
  _intcpt   =  0.0;
  _units    = sUnits;
  _start    = micros();
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

/** readSensor() returns a raw ADC value from the chosen analog sensor.
 *
 * @return {AnalogReading} the voltage on a single sensor
 */
AnalogReading
VernierAnalogSensor::readSensor()
{
  AnalogReading rc;

  rc.reading = 0.0;
  rc.adc = analogRead( _channel );
  rc.timestamp = micros()-_start;

  return rc;
}

/** readSensor() returns a single value from the chosen analog sensor
 *               but applies a conversion to the ADC value
 *
 * @return {AnalogReading} the voltage on a single sensor
 */
AnalogReading
VernierAnalogSensor::readValue()
{
  AnalogReading rc = readSensor();
  rc.reading = applyCalibration( rc.adc );

  return rc;
}

/** begin() reset the timer starting now!
 *
 */
void
VernierAnalogSensor::begin() {
  _start=micros();
}

/** readSensor() returns a single value from the chosen analog sensor
 *               but applies a conversion to the ADC value
 *
 * @return {char*} a pointer to a string of unit labels
 */
const char*
VernierAnalogSensor::sensorUnits() {
  return _units;
}
