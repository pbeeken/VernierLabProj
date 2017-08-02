/****************************************************************
*  ShieldControl
*  This encapsulates the control and state engine for the Arduino
*  management of the Vernier Shield. There should be only one
*  instance of this object which is the data acquisition tool
*  Tested and developed in Platformio 3.1.0
*  PBeeken ByramHills High School 7.12.2017
*
*  12 Jul 2017- begin development of a binary communication protocol
****************************************************************/
#ifndef ShieldCoontrol_h
#define ShieldCoontrol_h
#include <Arduino.h>

enum STATES {
    CLEAR_ALL = 'C', // System is reseting (rare)
    READY_STATE = 'R', // Armed: waiting for event to start taking data
    WAITING_STATE ='W', // Just listening for commands
    RUNNING_STATE = 'G', // Actively gathering data
};

class ShieldControl {

public:
   ShieldControl();

   char getState() { return _state; }
   void setState( char s ) { _state = s; }

   void trigMode( char tm ) { _trigMode = tm; }


   void sendStatus();

private:
   char _state;  // state of engine  READY, RUNNING or WAITING
   char _trigMode;



};

#endif
