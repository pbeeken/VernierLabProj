/****************************************************************
VernierButton
	This is a class designated to read the button on the
	SparkFun Vernier Shield.  The button can be used for a
	variety of user interface controls.

   Tested and developed in Platformio 3.1.0
   PBeeken ByramHills High School 9.1.2016

   Modified to fit VernierAnalogSensor refactoring
   PBeeken ByramHills High School 7.23.2017

****************************************************************/
#include <Arduino.h>
#include <VernierThermistor.h>
#include <math.h>
//#include <Streaming.h>

/** Constructor
 *    setup button read.
 */
VernierThermistor::VernierThermistor( int channel ) : VernierAnalogSensor( channel )
{
  static const char sUnits[] = "°C";
  _slope = 1.0;  // we don't really care because what we need to override is the
  _intcpt = 0.0; // whole calibration functon since the conversion is non-linear
  _units = sUnits;
}

/** Inputs ADC count from Thermistor and outputs Temperature in Celsius
 *  requires: include <math.h>
 * There is a huge amount of information on the web about using thermistors with the Arduino.
 * Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the
 * Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
 * thermistors.
 * This version utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
 *   for the themistor in the Vernier TMP-BTA probe:
 *    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
 *    Using these values should get agreement within 1 degree C to the same probe used with one
 *    of the Vernier interfaces
 *
 * Schematic:
 *   [Ground] -- [thermistor] -------- | -- [15,000 ohm resistor] --[Vcc (5v)]
 *                                     |
 *                                Analog Pin 0
 *	 For the circuit above:
 * Resistance = ( Count*RawADC /(1024-Count))
 */
#define RESISITOR 15000.0 //fixed resistor

float
VernierThermistor::applyCalibration(int adcValue) {
	// the measured resistance of your particular fixed resistor in
	// the Vernier BTA-ELV and in the SparkFun Vernier Adapter Shield
	// is a precision 15K resisitor
  float Resistance = RESISITOR/(1024-adcValue);
	Resistance *= adcValue;
  float tmp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
	tmp = 1 / (0.00102119 + (0.000222468 * tmp) + (0.000000133342 * tmp * tmp * tmp));
	tmp = tmp - 273.15;  // Convert Kelvin to Celsius
	return tmp;   // Return the temperature

//   float Temp;  // Dual-Purpose variable to save space.
// Resistance=( RESISTOR*adcValue /(1024-adcValue));
// tmp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
// tmp = 1 / (0.00102119 + (0.000222468 * tmp) + (0.000000133342 * tmp * tmp * tmp));
// tmp = tmp - 273.15;  // Convert Kelvin to Celsius

}
