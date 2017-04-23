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

class ShieldCommunication {
public:
   ShieldCommunication();

   void CollectCommand();
   bool isCommandComplete (){
      return isComplete;
   }

   int getParameter();
   bool isCommand(const char* tstCmd);
   void commandComplete (){
      resetCommand();
   }

protected:
   void resetCommand(){
      command.clear();
      parameter.clear();
      isComplete = false;
   }
private:
   String command;
   String parameter;

   bool   isComplete;
};

#endif
