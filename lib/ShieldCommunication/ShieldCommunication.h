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

// These are the commands.  The structure is such that the lowest two bits
// flags how many additional parameters are to be sent.
enum CMDS {
   // State changing commands
   HALT = 0x2C, GO   = 0x28, BTN  = 0x24,

   // Mode changine commands (what are we reading...)
   DIGMDE1 = 0x31, DIGMDE2 = 0x35,
   ANAMDE1 = 0x42,ANAMDE2 = 0x46, ANAMDEB = 0x4A,
   // (... start and stop conditions
   TRIGMODE = 0x52, STOPMODE = 0x56,

   // Interogation
   STATUS  = 0x50,

   // Get immediate reads
   IMMEDA105 = 0x60, IMMEDA205 = 0x64,
   IMMEDA110 = 0x68, IMMEDA210 = 0x6A,
   IMMEDD1 = 0x70,   IMMEDD2 = 0x74,
};

// Flags that mark the data sources in the high nibble of the data blob
enum SOURCES {
   DIG1 = 0x10,   DIG2 = 0x20,
   ANA105 = 0x30, ANA205 = 0x40,
   ANA110 = 0x50, ANA210 = 0x60,
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
   void sendStatus( char state); // communications status
   void sendStatus( const char* report ); // string from other object

   // senders
   void sendDataBlob( unsigned long time, unsigned int value, char channel );

   // basic getters
   char      getCommand() { return (int)_predicate; }
   char      getParamOne() { return _param1; }
   char      getParamTwo() { return _param2; }
   unsigned int  getParameters() { return makeWord(_param1, _param2); }

private:
   char _predicate;
   char _param1;
   char _param2;

   char _paramCount;  // COMPLETE BUILDING or READY
                        // -1       >0         ==0
   char _cmdCount;
};

#endif
