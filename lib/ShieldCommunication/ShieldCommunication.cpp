
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
*  07 Jul 2017- tested communication to two different interfaces
*  12 Jul 2017- begin development of a binary communication protocol
****************************************************************/

#include <ShieldCommunication.h>
#include <Streaming.h>

/**
 * Initialize object to receive
 */
ShieldCommunication::ShieldCommunication() {
   commandStatus = -1;
   // Command is complete, we are ready for another command
}

/**
 * Ask if the cocommand is still being built
 */
bool
ShieldCommunication::isInProgress() {
   return commandStatus>0;
}

/**
 * Ask if the command is complete and parameters are in their place.
 */
bool
ShieldCommunication::isCommandComplete() {
   return commandStatus==0;
}

/**
 * Ask if the command is ready to be built.
 */
bool
ShieldCommunication::isReadyToReceive() {
   return commandStatus!=0;
}


/** CollectCommand
 *  This routine is intended for the serialEvent stub where this object
 *  waits for and collects the commands.  Commands consist of the following
 *  elements which, in turn, control how this parser works:
 * OLD SCHOOL
 *  Commands start with 3 letters white space and a set of optional numbers
 *  terminated by a semicolon. Spaces, carriage returns,
 *  indeed all non-printables are ignored.
 * NEW PATTERN
 *  Command pattern is not simple as some require additional information.
 *  problem with binary communications is that it is hard to know when a
 *  communication is done except by counting characters so we need a
 *  timeout.
 */
void
ShieldCommunication::collectCommand() {
   // If the previous command wasn't handled then we don't do anything.
      while(Serial.available())
      {
         // delay(20);                //DEBUG
         // digitalWrite(13,HIGH);    //DEBUG
         // delay(20);                //DEBUG
         // digitalWrite(13,LOW);     //DEBUG
         // read a character
         char cc = (char)Serial.read();

         // if cmdStatus is -1 then this is the prime directive which
         // flags how many more bytes are to follow.
         if (commandStatus==-1) {
            commandStatus = cc & 0x03; // mask count for additional bytes
            command = cc;
//            Serial << "." << (char) cc << ".";   //DEBUG
            break;  // We are done here.
         }

         switch( commandStatus ) {
            case 0: // We should never get here. as we handled this above.
                     break;
            case 1: // One additional parameter
                     param1=cc;
                     // Serial << ">" << (char) cc << "<"; //DEBUG
                     commandStatus--;
                     break;
            case 2: // Two additional parameters
                     param2=cc;
                     // Serial << "}" << (char) cc << "{"; //DEBUG
                     commandStatus--;
                     break;
            case 3: // Nothing for this count currently.
                     badCommand(); // There are no 3 byte commands
                     break;
         }
   }
}

/**
 * Flag the command as having been handled successfully and are returning to wait
 */
void
ShieldCommunication::commandSuccessful() {
   Serial << "!"; //(int8_t)0x06; //ASCII ACK
//   Serial.write((int8_t)0x06);
   commandStatus=-1;
}

/**
 * Flag the command as being bad and return to getting a new command
 */
void
ShieldCommunication::badCommand() {
    Serial << "?"; // (int8_t)0x15; //ASCII NAK
    commandStatus=-1;
}

/**
 * Format and send current state
 */
void
ShieldCommunication::sendStatus(char state) {
   Serial << "Status: " << state;
   Serial << " -cmd: " << command << " (0x" << _HEX(command) << ")";
   switch ( command&0x03 ) {
      case 2:  Serial << " p2: " << param2 << " (0x" << _HEX(param2) << ")" << " [0b" << _BIN(param2) << "]";
      case 1:  Serial << " p1: " << param1 << " (0x" << _HEX(param1) << ")" << " [0b" << _BIN(param1) << "]";
   }
   Serial << endl;
}

int
ShieldCommunication::getCommand() {
   return (int)command;
}

int
ShieldCommunication::getParameter() {
   return (int)param1<<8 & (int)param2;
}
