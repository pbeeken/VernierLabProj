/****************************************************************
Vernier1DAccelerometer
	This is the subclass of VernierAnalogSensor which applies
	the specific conversions for the 1D accelerometer.

      Tested and developed in Platformio 3.1.0
      PBeeken ByramHills High School 9.1.2016

      Modified to fit VernierAnalogSensor refactoring
      PBeeken ByramHills High School 7.23.2017

****************************************************************/
#ifndef Vernier1DAccelerometer_h
#define Vernier1DAccelerometer_h

#include <VernierAnalogSensor.h>

class Vernier1DAccelerometer: public VernierAnalogSensor
{
  public:
    Vernier1DAccelerometer( int channel );			// Constructor. Mainly sets up pins.

    const static int BTA01 = VernierAnalogSensor::BTA01_5V;  // A0
    const static int BTA02 = VernierAnalogSensor::BTA02_5V;  // A2


  protected:
    // No need to override applyCalibration because this is a linear correction

};

#endif
