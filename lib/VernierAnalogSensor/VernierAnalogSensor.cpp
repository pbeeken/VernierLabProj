/****************************************************************
   VernierAnalogSensor
   This is the superclass designated to serve as the model for
   all other analog sensors. The best way to use this object is
   through the subclass associated with the specific sensor or
   experiment.

   This class was refactored to better match how we read the digital
   ports.

   Tested and developed in Platformio 3.1.0
   PBeeken ByramHills High School 9.1.2016

   17 Oct 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierAnalogSensor.h>
//#include <Streaming.h> // DEBUG

/** Constructor
 *    setup channels and initialize values.
 */
VernierAnalogSensor::VernierAnalogSensor( int channel )
{
        _channel     = channel;
        _slope       = 1.0;
        _intcpt      = 0.0;
        strcpy(_units, "raw");
        strcpy(_name, "Gen Analog");
        strcpy(_shortname, "GA");
        _trigState   = STATE::TS_HALT;
        setSampleRate(SAMPLERATES::S_10Hz);  // default: 10Hz
        setStopCondition( 100 );             // stop after 100 points
        setTrigger();                        // default: IMMEDIATE
        sync();                              // zero the clock
}

/**
 * raw immediate read of selected analog channel
 */
int
VernierAnalogSensor::readPort() {
        _absTime  = micros()-_start_us;
        return analogRead(_channel);
}


/**
 * poll the port and take data if the conditions are right
 */
bool
VernierAnalogSensor::pollPort() {

        // unsigned long dbg_st = micros(); // DEBUG

        // we aren't ready, don't do anything  exp. Takes <4μs
        if ( _trigState==STATE::TS_HALT ) return false;

        // Serial << "    1:" << ( micros() - dbg_st ) << endl; // DEBUG  4μS
        //               dbg_st = micros();

        // montor channels to see if trigger conditions are met. Takes <4μs
        if ( _trigState == STATE::TS_ARMED ) {
                if ( _trigCond == ATRIGCOND::TS_IMMEDIATE ) _trigState = STATE::TS_RUN;
                else if ( _trigCond == ATRIGCOND::TS_RISE_ABOVE && readPort()>_trigLevel ) _trigState = STATE::TS_RUN;
                else if ( _trigCond == ATRIGCOND::TS_FALL_BELOW && readPort()<_trigLevel ) _trigState = STATE::TS_RUN;
                _nextRead = micros()+_sampPeriod;
                return false;
        }

        // Serial << "    2:" << ( micros() - dbg_st ) << endl; // DEBUG
        //               dbg_st = micros();                     // DEBUG

        // test stop conditions. Takes 4μs
        // Simplified. We only limit by count
        if ( _stopCond != 0 ) {  // there is a stop condition
                // if ( _stopMethod ) { // based on time
                //         if ( _stopCond <= _absTime ) {
                //                 _trigState = STATE::TS_HALT;
                //         }
                // }
                // else {          // based on count *sub one to allow one more grab
                if ( _stopCond <= _count ) {
                    _trigState = STATE::TS_HALT;
                }
                // }
        }

        // Serial << "    3:" << ( micros() - dbg_st ) << endl; // DEBUG
        //               dbg_st = micros();                     // DEBUG

        // take data based on timing conditions  Takes 4-8μs for non-read, 128-136μs for a read (automatic)
        if ( _sampPeriod==0L ) { // act on a button press.
                // If button hit then report state
                if ( _btn.buttonIsDown() ) {
                        // trick for waiting unitl slow meat lets go.
                        while( _btn.buttonIsDown() ) ;
                        _rawReading = readPort();
                        _absTime  = micros()-_start_us;
                        _nextRead = micros()+_sampPeriod;
                        _count++;
                        return true;
                }
        } else if ( micros()>_nextRead ) {
                _rawReading = readPort();
                _absTime  = micros()-_start_us;
                _nextRead = micros()+_sampPeriod;
                _count++;

                // Serial << "    4r:" << ( micros() - dbg_st ) << endl; // DEBUG

                return true;
        }

        // Serial << "    4nr:" << ( micros() - dbg_st ) << endl; // DEBUG

        return false;
}

/**
 * set sample time.  A small accident revealed the fastest time the Arduino
 * can sample is 1528us (~1.5ms) per call. The input is a flag to set the
 * timer for specific intervals
 */
void
VernierAnalogSensor::setSampleRate( char intFlag ) {
        switch (intFlag) {
        case SAMPLERATES::S_30s:   _sampPeriod = 30000*1000L; break;
        case SAMPLERATES::S_10s:   _sampPeriod = 10000*1000L; break;
        case SAMPLERATES::S_5s:    _sampPeriod = 5000*1000L; break;
        case SAMPLERATES::S_2s:    _sampPeriod = 2000*1000L; break;
        case SAMPLERATES::S_1Hz:   _sampPeriod = 1000*1000L; break; // microseconds
        case SAMPLERATES::S_5Hz:   _sampPeriod = 200*1000L; break;
        case SAMPLERATES::S_10Hz:  _sampPeriod = 100*1000L; break;
        case SAMPLERATES::S_20Hz:  _sampPeriod = 50*1000L; break;
        case SAMPLERATES::S_40Hz:  _sampPeriod = 25*1000L; break;
        case SAMPLERATES::S_50Hz:  _sampPeriod = 20*1000L; break;
        case SAMPLERATES::S_100Hz: _sampPeriod = 10*1000L; break;
        case SAMPLERATES::S_200Hz: _sampPeriod = 5*1000L; break;
        case SAMPLERATES::S_500Hz: _sampPeriod = 2*1000L; break;
        case SAMPLERATES::S_1kHz:  _sampPeriod = 1*1000L; break;
        case SAMPLERATES::FASTEST: _sampPeriod = 1L; break; // 1 msec, practically: 1.5 msec per sample
        }
        sync();
}

/** setTrigger()
 * set the trigger conditions.  N.B. if a level is provided then it must be in terms
 *   of the raw digital value. The Arduino has 10bit ADCs which means 512 is 0V input
 *   regardless of range.
 **/
void
VernierAnalogSensor::setTrigger( int trigCond, int raw_value ) {
        _trigCond = trigCond;
        _trigLevel = raw_value;
        _trigState = STATE::TS_HALT;
}


/** applyCalibration() is a virtual function which, by default,
 *                     applies a linear correction to the raw
 *                     digital input to generate a more meaningful
 *                     value.  This is a virtual function.
 *
 * @param {adcValue} input int from ADC.
 */
float
VernierAnalogSensor::applyCalibration( int adcValue ) {
        return _slope * adcValue + _intcpt;
}

/** setStopCondition() sets the conditions for stopping data
 *                     acquisition. If useTime is true then the stopValue
 *                     is the time in msec (max 8 sec) otherwise it is a
 *                     the number of values to retrieve.
 *                      TODO: should be just count because the number of
 *                      points can be easily calculate from:
 *                      points = desiredTime / period
 */
void
VernierAnalogSensor::setStopCondition(int stopCount) {
//        _stopMethod = useTime;
        _stopCond = stopCount; // useTime ? 1000*(unsigned long)stopValue : stopValue;
}

// /** readSensor() returns a single value from the chosen analog sensor
//  *               but applies a conversion to the ADC value
//  *
//  * @return {AnalogReading} the voltage on a single sensor
//  */
// AnalogReading
// VernierAnalogSensor::readValue()
// {
//   AnalogReading rc = readSensor();
//   rc.reading = applyCalibration( rc.adc );
//
//   return rc;
// }

/** sync(start) synchronize clocks
 *  @pparam {syncTime} time in microseconds to sync clocks (allows for syncing multiple objects)
 */
void
VernierAnalogSensor::sync( unsigned long syncTime ) {
        _rawReading = 0;
        _start_us = syncTime > 0 ? syncTime : micros();
        _nextRead = _start_us + _sampPeriod;
        _count = 0L;
        _trigState = STATE::TS_HALT;
        _absTime = 0L;
}

unsigned long
VernierAnalogSensor::getCurrentTime() {
        return micros() - _start_us;
}

/** getStatus()
 * @returns String object with information
 */
String
VernierAnalogSensor::getStatus( const char* open ) {
        String msg(open);
        msg += "{";
        msg += "\"state\":";
        switch( _trigState ) {
                case STATE::TS_ARMED: msg += "\"A\""; break;
                case STATE::TS_RUN:   msg += "\"R\""; break;
                case STATE::TS_HALT:  msg += "\"H\""; break;
                }

        msg += ",\"period\":";    msg += _sampPeriod;   //msg += "µs ";

        msg += ",\"trigger\":";
        switch( _trigCond ) {
                case ATRIGCOND::TS_IMMEDIATE:   msg += "\"I\""; break;
                case ATRIGCOND::TS_FALL_BELOW:  msg += "\"F("; msg += _trigLevel; msg += ")\""; break;
                case ATRIGCOND::TS_RISE_ABOVE:  msg += "\"R("; msg += _trigLevel; msg += ")\""; break;
                }

        msg += ",\"stop\":"; msg += _stopCond;  //msg += _stopMethod ? "µs" : "#";
        msg += ",\"units\":"; msg += "\""; msg += _units; msg += "\"";
        msg += ",\"name\":"; msg += "\""; msg += _name; msg += "\"";
        msg += ",\"shortname\":"; msg += "\""; msg += _shortname; msg += "\"";

        msg += "}";
        return msg;
}
