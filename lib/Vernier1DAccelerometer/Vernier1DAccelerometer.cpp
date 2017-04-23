/****************************************************************
Vernier1DAccelerometer
	This is the subclass of VernierAnalogSensor which applies
	the specific conversions for the 1D accelerometer.

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

	17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <Vernier1DAccelerometer.h>

/** Constructor
 *    setup Accelerometer channel.
 */
Vernier1DAccelerometer::Vernier1DAccelerometer( int channel ) : VernierAnalogSensor( channel )
{
  static const char sUnits[] = "m/s²";
  _slope = -0.1134;
  _intcpt = +50.978;
  _units = sUnits;
}
