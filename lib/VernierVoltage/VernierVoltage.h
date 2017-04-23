/****************************************************************
VernierVoltage
	This is a class designated to read the analog voltage on the
	input directly.  The full scale is ±10V and the calibration
	converts to that value.

Tested and developed in Platformio 3.1.0
PBeeken ByramHills High School 9.1.2016


17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef VernierVoltage_h
#define VernierVoltage_h

#include <VernierAnalogSensor.h>

class VernierVoltage: public VernierAnalogSensor
{
  public:
    VernierVoltage( int channel );			// Constructor. Mainly sets up pins.

    const static int BTA01 = VernierAnalogSensor::BTA01_10V;  // A1
    const static int BTA02 = VernierAnalogSensor::BTA02_10V;  // A3

};

#endif
