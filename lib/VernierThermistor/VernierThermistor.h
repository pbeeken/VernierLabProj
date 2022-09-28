/****************************************************************
VernierThermistor
	This is a class designated to read the stainless steel therm.
   It also serves as a model for a non-linear conversion override.

   Tested and developed in Platformio 3.1.0
   PBeeken ByramHills High School 9.1.2016

   Modified to fit VernierAnalogSensor refactoring
   PBeeken ByramHills High School 7.23.2017

****************************************************************/
#ifndef VernierThermistor_h
#define VernierThermistor_h

#include <VernierAnalogSensor.h>

class VernierThermistor: public VernierAnalogSensor
{
  public:
    VernierThermistor( int channel );			// Constructor. Mainly sets up pins.

    const static int BTA01 = VernierAnalogSensor::BTA01_5V;  // A1
    const static int BTA02 = VernierAnalogSensor::BTA02_5V;  // A3

  protected:
    virtual float applyCalibration( int adcValue );

};

#endif
