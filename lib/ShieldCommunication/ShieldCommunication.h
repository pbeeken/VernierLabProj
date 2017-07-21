/****************************************************************
*  ShieldCommunication
*  This encapsulates the serial communication from the host machine. The Serial
*  port is managed outside this object through the establishment of a baud rate
*  and .begin notification.  After that this object is meant to do all the work.
*
*  Tested and developed in Platformio 3.1.0
*  PBeeken ByramHills High School 9.1.2016
*
*  10 Dec 2016- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef ShieldCommunication_h
#define ShieldCommunication_h
#include <Arduino.h>

enum CMDS {
   HALT = 0x2C,
   GO   = 0x28,
   BTN  = 0x24,

   DIGMDE1 = 0x31,
   DIGMDE2 = 0x35,

   ANAMDE1 = 0x42,
   ANAMDE2 = 0x46,
   ANAMDEB = 0x4A,

   STATUS  = 0x50,
   TRIGMODE = 0x52,

   IMMEDA1 = 0x60,
   IMMEDA2 = 0x64,
   IMMEDD1 = 0x70,
   IMMEDD2 = 0x74,

};

class ShieldCommunication {

public:
   ShieldCommunication();

   void collectCommand();
   bool isReadyToBuild();
   bool isCommandComplete();
   bool isCommandBuilding();
   void commandSuccessful();
   void badCommand();
   void sendStatus( char state);
   int  getCommand();
   int  getParameter();

private:
   char _command;
   char _param1;
   char _param2;

   char _commandStatus;  // COMPLETE BUILDING or READY
                        // -1       >0             ==0
   char _cmdCount;
};

#endif
