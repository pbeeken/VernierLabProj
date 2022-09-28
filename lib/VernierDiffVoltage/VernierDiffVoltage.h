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
#ifndef VernierDiffVoltage_h
#define VernierDiffVoltage_h

#include <VernierAnalogSensor.h>

class VernierDiffVoltage: public VernierAnalogSensor
{
  public:
    VernierDiffVoltage( int channel );			// Constructor. Mainly sets up pins.

    const static int BTA01 = VernierAnalogSensor::BTA01_5V;  // A0
    const static int BTA02 = VernierAnalogSensor::BTA02_5V;  // A2

};

#endif
