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
#include <VernierVoltage.h>

/** Constructor
 *    setup button read.
 */
VernierVoltage::VernierVoltage( int channel ) : VernierAnalogSensor( channel )
{
  static const char sUnits[] = "V";
 // 10bit dig gives the following -10V-->0 to 10V-->1024,
  _slope = 20.0/1024;  // voltage change per raw bit reading
  _intcpt = -10.0;   // offset of input op-amp
  _units = sUnits;
}
