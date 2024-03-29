/****************************************************************
*  ShieldControl
*  This encapsulates the control and state engine for the Arduino
*  management of the Vernier Shield. There should be only one
*  instance of this object which is the data acquisition tool
*  Tested and developed in Platformio 3.1.0
*  PBeeken ByramHills High School 7.12.2017
*  12 Jul 2017- begin development of a binary communication protocol
*  ShieldControl was dropped in favor of VernierShieldCommunication which implements
*     a binary communication protocol and fully operational firmware with control by
*     a connected python app.
****************************************************************/
#ifndef ShieldCoontrol_h
#define ShieldCoontrol_h
#include <Arduino.h>

enum STATES {
    CLEAR_ALL = 'C', // System is reseting (rare)
    READY_STATE = 'R', // Armed: waiting for event to start taking data
    WAITING_STATE ='W', // Start looking for trigger event that will start DA
    RUNNING_STATE = 'G', // Actively gathering data
};

class ShieldControl {

public:
   ShieldControl();

   char getState() { return _state; }
   void setState( char s ) { _state = s; }

   void setMode( char m ) { _mode = m; }
   char getMode() { return _mode; }


   void sendStatus(int maj_rev, int min_rev);

private:
   char _state;  // state of engine  READY, RUNNING or WAITING
   char _mode;



};

#endif
