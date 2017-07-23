/****************************************************************
VernierDiffVoltage
	This is a class designated to read the analog voltage on the
	input from the differential amplifier.  The full scale is ±5V
	an the calibration converts to that value.


      Tested and developed in Platformio 3.1.0
      PBeeken ByramHills High School 9.1.2016

      Modified to fit VernierAnalogSensor refactoring
      PBeeken ByramHills High School 7.23.2017

****************************************************************/
#include <Arduino.h>
#include <VernierDiffVoltage.h>

/** Constructor
 *    setup button read.
 */
VernierDiffVoltage::VernierDiffVoltage( int channel ) : VernierAnalogSensor( channel )
{
  static const char sUnits[] = "V";
  _slope = 10.0/1024;
  _intcpt = -5.0;
  _units = sUnits;
}
