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
#ifndef VernierAnalogSensor_h
#define VernierAnalogSensor_h

// All readings use this object for storing analog values.
struct AnalogReading {
  int           adc;  // straigh ADC reading 0 - 1024
  unsigned long timestamp;
  float         reading;     // calibrated value
};


class VernierAnalogSensor
{
  public:
    VernierAnalogSensor( int channel );			// Constructor. Mainly sets up pins.

    // 	Basic tools, reads a sensor and returns the time index along
    AnalogReading readSensor();  // read raw 'voltage' digital value
    AnalogReading readValue();   // read processed value (applies conversion)
    const char*   sensorUnits(); // return sensor's units
    void          begin();       // start the clock

	// constants for channels
    const static int BTA01_5V  = 14;  // A0
    const static int BTA01_10V = 15;  // A1
    const static int BTA02_5V  = 16;  // A2
    const static int BTA02_10V = 17;  // A3

	// elements for subclassing
  protected:
  	// default linear calibration
    virtual float applyCalibration( int adcValue );
    // default quantities for subclasses
    float        _slope;
    float        _intcpt;
    const char*  _units;

	// exclusive to this object
  private:
    int           _channel;
    unsigned long _start;

};

#endif
