
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

#include <ShieldCommunication.h>

// union converter {
//    struct {
//       byte msb: 8;
//       byte lsb: 8;
//    } parts;
//    int whole;
// };


ShieldCommunication::ShieldCommunication(){
   isComplete = false;
   command.reserve(5);
   parameter.reserve(5);
   isComplete = false;
}

int
ShieldCommunication::getParameter(){
   return parameter.toInt();
}

bool
ShieldCommunication::isCommand (const char* tstCmd){
   return command == String(tstCmd);
}

/** CollectCommand
 *  This routine is intended for the serialEvent stub where this object
 *  waits for and collects the commands.  Commands consist of the following
 *  elements which, in turn, control how this parser works:
 *  Commands start with 3 letters white space and a set of optional numbers
 *  terminated by a semicolon. Spaces, carriage returns,
 *  indeed all non-printables are ignored.
 */
void
ShieldCommunication::CollectCommand (){
   // we are a state engine. If the previous command hasn't been
   // consumed we don't do anything here.
   if(!isComplete)
      while(Serial.available())
      {
         char cc = (char)Serial.read();
         if(isalpha(cc))
            command += cc;
         if(isdigit(cc))
            parameter += cc;
         if(cc == ';')
         {
            if(command.length() > 0 && parameter.length() <= 4)
            {
               isComplete = true;
               command.toUpperCase();
            }
            else                       // invalid elements clear and start again
            {
               resetCommand();
            }
         }
      }
}
