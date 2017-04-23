/****************************************************************
VernierDiffVoltage
	This is a class designated to read the analog voltage on the
	input from the differential amplifier.  The full scale is ±5V
	an the calibration converts to that value.

Tested and developed in Platformio 3.1.0
PBeeken ByramHills High School 9.1.2016


17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierVoltage.h>

/** Constructor
 *    setup button read.
 */
VernierVoltage::VernierVoltage( int channel ) : VernierAnalogSensor( channel )
{
  static const char sUnits[] = "V";
  _slope = 0.01912;
  _intcpt = -9.806;
  _units = sUnits;
}
