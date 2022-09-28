/****************************************************************
*  ShieldCommunication
*  This encapsulates the serial communication from the host machine. The Serial
*  port is managed outside this object through the establishment of a baud rate
*  and .begin notification.  After that this object is meant to do all the work.
*
*  The commands structure and reasoning is outlined in a separate document.
*
*  Tested and developed in Platformio 3.1.0
*  PBeeken ByramHills High School 9.1.2016
*
*  10 Dec 2016- P. Beeken, Byram Hils High School
****************************************************************/
#ifndef ShieldCommunication_h
#define ShieldCommunication_h
#include <Arduino.h>

// separate header with all the command codes
#include <ShieldCommunicationCmds.h>

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
   void sendDataBlob( int index, unsigned long time, int rawValue, int channel );
   void sendString( String msg );

   // basic getters
   char            getCommand() { return (int)_predicate; }
   unsigned long   getParameter();
   char            getParameter(int i);

private:
   char _predicate;
   char _param[3];

   int _paramCount;  // COMPLETE BUILDING or READY
                        // -1       >0         ==0
   char _cmdCount;
};

#endif
