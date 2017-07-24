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

TODO: Build a virtual table of functions to be run based on commands?
the compiler can fashion a nul pointer to functions declared in the
following way:
extern void someFunction(void) __attribute__((weak));
if the function is never defined then the pointer to it is set to nil
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

   // called in the serialEvent function to collect characters into
   //   a 'command'. It parses the characters and isolates the predicate
   //   from the parameters (if any)
   void collectCommand();
   // ready to build the command (go get a new command)
   bool isReadyToBuild();
   // command has been recieved and parsed and ready to process
   bool isCommandComplete();
   // command is in the process of being built
   bool isCommandBuilding();
   // return a message that command was understood
   void commandSuccessful();
   // return a message that the command was not understood
   void badCommand();
   // sent detailed information on the current command
   void sendStatus( char state);

   // senders
   void sendDigitalData( unsigned long time, unsigned int state );
   void sendAnalogData( unsigned long time, unsigned int value );

   // basic getters
   char      getCommand() { return (int)_predicate; }
   char      getParamOne() { return _param1; }
   char      getParamTwo() { return _param2; }
   uint16_t  getParameters() { return makeWord(_param1, _param2); }

private:
   char _predicate;
   char _param1;
   char _param2;

   char _paramCount;  // COMPLETE BUILDING or READY
                        // -1       >0             ==0
   char _cmdCount;
};

#endif
