/*
VernierAnalogAutoID (v 2013.12)
Reads the information to AutoID a Vernier BTA sensor with digital AutoID,
and resistor ID sensors including Vernier Stainless Steel Temperature Probe (TMP-BTA).
It should read the +/-10 volt Voltage Probe correctly also.
This version does all tests for resistorID sensors first, then
turns on the I2C clock and tests for digital autoID sensors.)

Note that this sketch handles multiple pages of sensor calibrations.

When used with the SparkFun Vernier Interface Shield, this program will AutoID
two different sensors on BTA1 and BTA2. With homemade, breadboard
connections, it will work with only one sensor.

After the AutoID:
Assuming Vernier analog (BTA) Sensors are connected to the BTA connectors,
this sketch displays the time and sensor readings on the Serial Monitor.
As written, the readings will be displayed every second.
Change the variable TimeBetweenReadings to change the rate.
 See www.vernier.com/arduino for more information.
*/

//#define DEBUG
#ifdef DEBUG
#include <Streaming.h>
#endif

#include <Arduino.h>
#include <Wire.h>
#include <VernierDetect.h>

#define MUX_LSB 10
#define MUX_MSB 11
#define DEVICE  0x50     // I2C Address for sensors - used for calibration data

////////////////////////////////////////
//int device=0x50;
uint8_t floatbyte[2];


VernierDetect::VernierDetect( int channel ) {
  _channel = channel;
  pinMode(MUX_LSB, OUTPUT);  // multiplexer pins for AutoID
  pinMode(MUX_MSB, OUTPUT);  // multiplexer pins for AutoID
}

bool
VernierDetect::detectAnalogSensor() {
  switch( _channel ) {
    case BTA01:
          // Read BTA1 Sensor
          digitalWrite(MUX_LSB, LOW); //set multiplexer for BTA1
          digitalWrite(MUX_MSB, LOW);
          if ( !BTAResistorSensorID() )
            DigitalSensorID(); // if no resistorID, check for digital ID
          break;
    case BTA02:
          // Read BTA2 Sensor
          digitalWrite(MUX_LSB, HIGH); //set multiplexer for BTA2
          digitalWrite(MUX_MSB, LOW);
          if ( !BTAResistorSensorID() )
            DigitalSensorID(); // if no resistorID, check for digital ID
          break;
    }
    return _sensor!=0;
}

/***
 * TODO: Complete code
 **/
bool
VernierDetect::DigitalSensorID() {
  int i, rc;
  char sensordata [128];
  // check for digital ID sensor:
  for (i = 0; i < 128; i++) {// clear our digital ID sensor data
    sensordata[i] = 0;
    }

#ifdef DEBUG
  Serial << "DigID" << endl;
#endif

  //Now check for Digital AutoID sensor:
  Wire.begin(); // join i2c bus (address optional for master) !!!
  //Reading DEVICE first time... ;
  Wire.beginTransmission(DEVICE);  // Now we're going to read it back
  Wire.write(0x0);                 // Sending address 0, so it knows where we'll want
  Wire.endTransmission();          // to read from
  rc = Wire.requestFrom(DEVICE, 32); // Start new transmission and keep reading for 128 bytes
  i = 1;
#ifdef DEBUG
   Serial << "Wire Request In: " << rc << endl;
#endif
  while (rc > 1)
  {
    rc = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i] = c;
    i++;
  }
  //Reading DEVICE second time... ;
  Wire.beginTransmission(DEVICE); // Now we're going to read it back
  Wire.write(0x20);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();       // to read from
  rc = Wire.requestFrom(DEVICE, 32);  // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (rc > 1)
  {
    rc = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 32] = c;
    i++;
  }
  //Reading DEVICE third time... ;
  Wire.beginTransmission(DEVICE); // Now we're going to read it back
  Wire.write(0x40);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();         // to read from
  rc = Wire.requestFrom(DEVICE, 32); // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (rc > 1)
  {
    rc = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 64] = c;
    i++;
  }
  //Reading DEVICE a forth time... ;
  Wire.beginTransmission(DEVICE); // Now we're going to read it back
  Wire.write(0x60);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();       // to read from
  rc = Wire.requestFrom(DEVICE, 32);  // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (rc > 1)
  {
    rc = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 96] = c;
    i++;
  }
  //      Print out array:  // remove *'s to get this display for diagnostics

#ifdef DEBUG

  Serial << "array: ";
  for (i = 1; i <= 128; i++)
  {
    Serial << i << "\t " << _DEC( sensordata[i] ) <<<< "\t " << char(sensordata[i]) << endl;
  }
#endif

  //******************************************************************
  //Determine sensor number:
  //  VoltageID=-1;// indicated resistor ID not used
  _sensor = sensordata[2];

  //Determine the sensor name:
  _name[0] = '\0';
  for (i = 0; i < 20; i++)  {
    char c =  sensordata[i + 9]; // 9 byte offset
    _name[i] += c;
  }
  _name[i] += '\0';    //add terminating character

  //Determine the short name:
  _shortname[0] = '\0';
  for (i = 0; i < 12; i++) {
    char c =  sensordata[i + 29]; // 29 byte offset
    _shortname[i] += c;
  }
  _shortname[i] += '\0';    //add terminating character

  //Determine the calibration equation type:
  _eqtype = sensordata[57];

  //Determines the  calibration page:
  _page = sensordata[70];

  // the code below uses the calibration page set:
  // _intercept starts at 71 for page 1, 90 for p2, and 109 for p3

  //Determines intercept:
  // for (i = 0; i < 4; i++) {
  //   floatbyte[i] = sensordata[i + 71 + (_page) * 19];
  // }
  memcpy( &_intercept, &(sensordata[i + 71 + (_page) * 19]), 4 );
  // float b = *(float*) &floatbyte;
  // _intercept = b;

  //Determines slope:
  // slope starts at 75 for page 1, 94 for p2, and 113 for p3
  // for (i = 0; i < 4; i++)
  // {
  //   floatbyte[i] = sensordata[i + 75 + (_page) * 19];
  // }
  memcpy( &_slope, &(sensordata[i + 75 + (_page) * 19]), 4 );
  // float m = *(float*) &floatbyte;
  // _slope = m;

  //Determines the units:
  // the cryptic code in the next line just uses the
  // correct bytes, depending on the page selected.
  // units start at 83 for page 1, 102 for p2, and 121 for p3
  for (i = 0; i < 5; i++) {
    char c =  sensordata[83 + i + (_page) * 19];
    _units[i] += c;
  }
  _units[i] = '\0';    //add terminating character

  return _sensor!=0;
} //end of checking for digital ID sensor

/***
 * Vernier sensors can be detected in one of two ways: older sensors
 * have a built in resistor which can be detected via this routine.
 * Newer digital sensors respond to a i2c query contained in another
 * routine.
 **/
bool
VernierDetect::BTAResistorSensorID() {
  float VCC = 5.0;
  float VoltageID;
  /* code */
  _name[0] = '\0'; // clear name string
  _units[0] = '\0';
  _shortname[0] = '\0'; // clear name string
  _sensor = 0;
  // short delay?
  delay (10);
  int CountID = analogRead(A5);
  VoltageID = CountID / 1023.0 * VCC;// convert from count to voltage
#ifdef DEBUG
  Serial << "AnID vID: " << VoltageID << endl;
#endif
  if ( (VoltageID>0.86) & (VoltageID<0.95) ) _sensor = 1; //Thermocouple
  if ( (VoltageID>3.83) & (VoltageID<3.86) ) _sensor = 2; // Voltage +/-10 V
  if ( (VoltageID>1.92) & (VoltageID<2.13) ) _sensor = 3; // TI Current Probe (not used)
  if ( (VoltageID>1.18) & (VoltageID<1.30) ) _sensor = 4; //Reistance
  if ( (VoltageID>3.27) & (VoltageID<3.68) ) _sensor = 5; //Extra-Long Temperature Probe
  if ( (VoltageID>4.64) & (VoltageID<4.73) ) _sensor = 8; //Differential Voltage
  if ( (VoltageID>4.73) & (VoltageID<4.82) ) _sensor = 9; //Current
  if ( (VoltageID>2.38) & (VoltageID<2.63) ) _sensor = 10; //Stainless Steel or Surface Temperature Probe
  if ( (VoltageID>2.85) & (VoltageID<3.15) ) _sensor = 11; // Voltage 30 V
  if ( (VoltageID>1.52) & (VoltageID<1.68) ) _sensor = 12; //TILT, TI Light Sensor
  if ( (VoltageID>0.43) & (VoltageID<0.48) ) _sensor = 13; //Exercise Heart Rate
  if ( (VoltageID>4.08) & (VoltageID<4.16) ) _sensor = 14; //Raw Voltage
  if ( (VoltageID>0.62) & (VoltageID<0.68) ) _sensor = 15; //EKG
  if ( (VoltageID>4.81) & (VoltageID<4.89) ) _sensor = 16; // Accelerometer
  if ( (VoltageID>4.32) & (VoltageID<4.40) ) _sensor = 17; //CO2
  if ( (VoltageID>4.50) & (VoltageID<4.59) ) _sensor = 18; //Oxygen


  switch (_sensor)
  {
    case 1:
      strcpy( _name, "Thermocouple" ) ;
      strcpy( _units, "°C" ) ;
      strcpy( _shortname, "TC" );
      _slope = -2.45455;
      _intercept = 6.2115;
      _page = 1; //calibration storage p (1,2, or 3)
      _eqtype = 1;
      break;
    case 2:
      strcpy( _name, "Voltage +/- 10V" ) ;  //!!! do not change this name or you will mess up the code of the loop
      strcpy( _units, "V" ) ;
      strcpy( _shortname, "V10" );
      _slope = 4; //note correction for Sparkfun circuit done in calculation of Voltage!!
      _intercept = -10;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 3:
      strcpy( _name, "Current" ) ;
      strcpy( _units, "A" ) ;
      strcpy( _shortname, "i" );
      _slope = -2.665;
      _intercept = 6.325;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 4:
      strcpy( _name, "Resistance" ) ;
      strcpy( _units, "Ω" ) ;
      strcpy( _shortname, "R" );
      _slope = -2.5;
      _intercept = 6.25;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 8:
      strcpy( _name, "Diff Voltage" ) ;
      strcpy( _units, "V" ) ;
      strcpy( _shortname, "∆V" );
      _slope = -2.5;
      _intercept = 6.25;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 9:
      strcpy( _name, "Current" ) ;
      strcpy( _units, "A" ) ;
      strcpy( _shortname, "i" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 10:
      strcpy( _name, "Temperature" ) ;
      strcpy( _units, "°C" ) ;
      strcpy( _shortname, "T" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 11:
      strcpy( _name, "Temperature" ) ;
      strcpy( _units, "°C" ) ;
      strcpy( _shortname, "T" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 12:
      strcpy( _name, "Light" ) ;
      strcpy( _units, "b" ) ;
      strcpy( _shortname, "B" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 13:
      strcpy( _name, "Heart Rate" ) ;
      strcpy( _units, "V" ) ;
      strcpy( _shortname, "HR" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 14:
      strcpy( _name, "Voltage" ) ;
      strcpy( _units, "V" ) ;
      strcpy( _shortname, "V" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 15:
      strcpy( _name, "EKG" ) ;
      strcpy( _units, "V" ) ;
      strcpy( _shortname, "EKG" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 16:
        strcpy( _name, "Accelerometer" ) ;
        strcpy( _units, "m/s²" ) ;
        strcpy( _shortname, "Acc" );
        _slope = 22.924;
        _intercept = -51.751;
        _page = 1; //calibration storage page
        _eqtype = 1;
        break;
    case 17:
      strcpy( _name, "Carbon Dioxide" ) ;
      strcpy( _units, "ppm" ) ;
      strcpy( _shortname, "CO2" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    case 18:
      strcpy( _name, "Oxygen" ) ;
      strcpy( _units, "%" ) ;
      strcpy( _shortname, "O2" );
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
    default:
      strcpy( _name, "nothing on BTA" ) ;
      _sensor = 0; //
      _units[0] = '\0' ;
      _shortname[0] = '\0';
      _slope = 1;
      _intercept = 0;
      _page = 1; //calibration storage page
      _eqtype = 1;
      break;
  } // end of switch case

  return _sensor!=0;
} //end of BTA resistor check

// unsigned int BAUD_RATE = 9600;  // set data rate for Serial monitor to be the fastest possible.
//
// int dataRate = 60;        // set # of samples per second.
// int duration = 15000;      // set the data collection duration in milliseconds
//                           // default value is set to 5 seconds or 5000 milliseconds
// char delimiter = '\t';
//
// // Variables used in the code for calculations
// unsigned long timeRef;    // reference for starting time
// unsigned long timeInterval;
//
// unsigned long ReadingNumber; // index for data counter


// void setup()
// {
//   pinMode(ledPin, OUTPUT);
//   pinMode(buttonPin, INPUT_PULLUP);
//   pinMode(MUX_LSB, OUTPUT);  // multiplexer pins for AutoID
//   pinMode(MUX_MSB, OUTPUT);  // multiplexer pins for AutoID
//
//   timeInterval = 1000 / dataRate;
//
// #ifdef PLX_DAQ
//   {
//     delimiter = ',';
//     BAUD_RATE = 56000;  // max speed for PLX_DAQ that matches with Arduino's data rates
//   }
// #endif
//   Serial.begin(BAUD_RATE);
//   digitalWrite(ledPin, LOW);
//
//   device = 0x50;     // I2C Address for sensors - used for calibration data
//   Serial.println("");
//
//   // Read BTA1 Sensor
//   digitalWrite(MUX_LSB, LOW); //set multiplexer for BTA1
//   digitalWrite(MUX_MSB, LOW);
//   BTAResistorSensorID(0);
//
//   // Read BTA2 Sensor
//   digitalWrite(MUX_LSB, HIGH); //set multiplexer for BTA2
//   digitalWrite(MUX_MSB, LOW);
//   BTAResistorSensorID(1);
//
//   Wire.begin(); //start I2C communication
//
//   // Read BTA1 Sensor
//   digitalWrite(MUX_LSB, LOW); //set multiplexer for BTA1
//   digitalWrite(MUX_MSB, LOW);
//   if (_sensor[0] == 0) DigitalSensorID(0); // if no resistorID, check for digital ID
//
//   // Read BTA2 Sensor
//   digitalWrite(MUX_LSB, HIGH); //set multiplexer for BTA2
//   digitalWrite(MUX_MSB, LOW);
//   if (_sensor[1] == 0) DigitalSensorID(1); // if no resistorID, check for digital ID
//
//
//   PrintSensorInfo(0);// this line can be commented out if you do not need all this info!!!
//   PrintSensorInfo(1);// this line can be commented out if you do not need all this info
//
//   PrintHeaderInfo();
//   ReadingNumber = 0;
//   timeRef = millis();
//
// }

// void loop()
// {
//   int Count[2]; //reading from 0 to 5 volt input
//   int AltCount[2] ;//reading from -10 to +10V input
//   float Voltage[2];
//   float SensorReading[2];
//   unsigned long currTime;
//
//   currTime = millis();
//   if ((currTime - timeRef) <= (duration))     // controls the duration of the data collection
//   {
//     if (currTime >= ReadingNumber * timeInterval + timeRef) // controls so only runs once per timeInterval
//     {
//       digitalWrite(ledPin, HIGH); //turn on LED
// #ifdef PLX_DAQ
//       {
//         Serial.print("DATA,");
//       }
// #endif
//
//       Serial.print((currTime - timeRef) / 1000.0, 3);
//       for (int Channel = 0; Channel <= 1; Channel++)
//       {
//         Serial.print(delimiter); //tab character
//         if (_name[Channel] == "Voltage +/- 10V")
//         {
//           AltCount[0] = analogRead(A1); //read both +/- 110 volt lines
//           AltCount[1] = analogRead(A3);
//           // convert from count to raw voltage if using 10 volt range:
//           Voltage[Channel] = AltCount[Channel] / 1023.0 * 5.0 ;
//         }
//         else
//         {
//           Count[0] = analogRead(A0); //read both analog lines
//           Count[1] = analogRead(A2);
//           // convert from count to raw voltage on 0 to 5 range:
//           Voltage[Channel] = Count[Channel] / 1023.0 * 5.0;
//         }
//         SensorReading[Channel] = _intercept[Channel] + Voltage[Channel] * _slope[Channel];
//         //special calibration for thermistor temperture probe:
//         if (_sensor[Channel] == 10) SensorReading[Channel] = Thermistor(Count[Channel]);
//         Serial.print(SensorReading[Channel], 3);
//       } // end of going through the channels
//
//       Serial.println(" ");
//       digitalWrite(ledPin, LOW);// LED on D13 flashes once per readng
//       ReadingNumber++;
//     }
//   }
//   else
//   {
//     digitalWrite(ledPin, LOW);
//
//     PrintHeaderInfo();
//     // reset counters and timeRef
//     digitalWrite(ledPin, HIGH);
//     ReadingNumber = 0;
//     timeRef = millis();
//   }
// } // end



// void PrintHeaderInfo()
// {
//   Serial.println();
//   Serial.println("**************************************************");
//   Serial.println("*   Push the D12 button to start collection.     *");
//   Serial.println("*      Use reset button to reset / stop          *");
//   Serial.println("**************************************************");
//   while (digitalRead(buttonPin) == HIGH)
//   {
//     // holding loop until the button goes low.
//   }
//   Serial.println("Vernier Format 2");
//   Serial.println("Sensor Readings taken using Ardunio");
//   Serial.println("Data Set");
//
// #ifdef PLX_DAQ
//   {
//     Serial.println("CLEARDATA");
//     Serial.print("LABEL,");
//   }
// #endif
//
//   Serial.print("Time");
//   for (int chan = 0; chan <= 1; chan++)
//   {
//     Serial.print(delimiter); //tab character
//     //print sensor name:
//     Serial.print (_shortname[chan]);
//   }
//   Serial.println("");
//
//   Serial.print("seconds");
//
//   for (int chan = 0; chan <= 1; chan++)
//   {
//     Serial.print("\t"); //tab character
//     //print sensor name:
//     Serial.print (_units[chan]);
//   }
//   Serial.println ("");
// }
//
//
// void PrintSensorInfo(int Channel)
// { // print out information about sensor:
//   //(This code is commented out, but add it for more feedback)
//   Serial.println(" ");
//   Serial.print("BTA Connector ");
//   Serial.println(Channel);
//   Serial.print("sensor ID number: ");
//   Serial.println(_sensor[Channel]);
//   Serial.print("ID voltage level: ");
//   Serial.println(VoltageID);
//   Serial.print("sensor name: ");
//   Serial.println (_name[Channel]);
//   Serial.print("sensor short name: ");
//   Serial.println (_shortname[Channel]);
//   Serial.print("calibration page: ");
//   Serial.println(_page[Channel]);
//   Serial.print("calibration equation type: ");
//   Serial.println(_eqtype[Channel]);
//   Serial.print("intercept: ");
//   Serial.println (_intercept[Channel]);
//   Serial.print("slope ");
//   Serial. println(_slope[Channel]);
//   Serial.print("units: ");
//   Serial.println (_units[Channel]);
// }// end of PrintSensorInfo




float
VernierDetect::convertInput( int inputReading ) {
  // We start by taking an digital value from the input and applying the
  // convesion.
  float VCC = 5.0;

  float rawVoltage = inputReading / 1023.0 * VCC;
  return _slope*rawVoltage + _intercept;
}



// float Thermistor(int Raw) //This function calculates temperature from ADC count
// {
//   /* Inputs ADC count from Thermistor and outputs Temperature in Celsius
//   *  requires: include <math.h>
//   * There is a huge amount of information on the web about using thermistors with the Arduino.
//   * Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the
//   * Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
//   * thermistors.
//   * This version utilizes the Steinhart-Hart Thermistor Equation:
//   *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
//   *   for the themistor in the Vernier TMP-BTA probe:
//   *    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
//   *    Using these values should get agreement within 1 degree C to the same probe used with one
//   *    of the Vernier interfaces
//   *
//   * Schematic:
//   *   [Ground] -- [thermistor] -------- | -- [15,000 ohm bridge resistor] --[Vcc (5v)]
//   *                                     |
//   *                                Analog Pin 0
//   *
//
//   For the circuit above:
//   * Resistance = ( Count*RawADC /(1024-Count))
//    */
//   long Resistance;
//   float Resistor = 15000; //bridge resistor
//   // the measured resistance of your particular bridge resistor in
//   //the Vernier BTA-ELV this is a precision 15K resisitor
//   float Temp;  // Dual-Purpose variable to save space.
//   Resistance = ( Resistor * Raw / (1024 - Raw));
//   Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
//   Temp = 1 / (0.00102119 + (0.000222468 * Temp) + (0.000000133342 * Temp * Temp * Temp));
//   Temp = Temp - 273.15;  // Convert Kelvin to Celsius
//   return Temp;                                      // Return the Temperature
// }
