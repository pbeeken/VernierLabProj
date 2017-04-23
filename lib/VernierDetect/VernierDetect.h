/****************************************************************
VernierDetect
	This is a class designated to read the resistor value and
  determine the type of sensor and it's conversions.
  <b>Time for some frank talk.</b> I don't think this module will be
  used very much given the philosophy of why I am building this
  package.  The idea is that the students learn how to compose
  data acquisition tools based on the instrumentation they choose.
  This means they know what device they are plugging in to what
  port and recognize that the values they are getting are voltages
  that might be converted to readings if there is a proper 
  conversion table.  Instead of this object, therefore, the students
  will be instantiating objects which handle specific sensors.
****************************************************************/
#ifndef VernierDetect_h
#define VernierDetect_h

class VernierDetect
{

  public:
    VernierDetect( int channel );			// Constructor. Mainly sets up pins.

    bool  detectAnalogSensor();
    float convertInput( int val );


    // getters for idetifying information
    char* getName() { return _name; }
    char* getShortname() { return _shortname; }
    char* getUnits() { return _units; }




    // const static int DigBTD01 = 001;
    // const static int DigBTD02 = 002;
    const static int BTA01 = 003;
    const static int BTA02 = 004;

  private:

    bool BTAResistorSensorID();   // Older sensors use a resisitor
    bool DigitalSensorID();       // Newer digital sensors return a dataset from an I2C

    int   _channel;
    int   _sensor; //integer indicating sensor number'
    char  _name[30];
    char  _shortname[14];
    char  _units[7];
    float _intercept;
    float _slope;
    int   _page;
    int   _eqtype;

//    float VCC= 5.00;// "5 volt" power supply voltage used in resistor ID section

};

#endif
