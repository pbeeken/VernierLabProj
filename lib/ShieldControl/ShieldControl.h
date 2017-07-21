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

class ShieldControl {

public:
   ShieldControl();

   byte getState() { return state; }
   void setState( byte s ) { state = s; }



private:
   byte state;  // state of engine  READY, RUNNING or WAITING




};

#endif
