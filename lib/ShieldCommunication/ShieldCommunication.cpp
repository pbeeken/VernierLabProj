
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
 **/
ShieldCommunication::ShieldCommunication() {
   _paramCount = -1;
   // Command is complete, we are ready for another command
}

/**
 * Ask if the cocommand is still being built
 **/
bool
ShieldCommunication::isCommandBuilding() {
   return _paramCount>0;
}

/**
 * Ask if the command is complete and parameters are in their place.
 **/
bool
ShieldCommunication::isCommandComplete() {
   return _paramCount==0;
}

/**
 * Ask if the command is ready to be built.
 **/
bool
ShieldCommunication::isReadyToBuild() {
   return _paramCount!=0;
}


/** CollectCommand
 *  This routine is intended for the serialEvent stub where this object
 *  waits for and collects the 'command.'  Commands consist of the
 *  instuction predicate and parameters. With ASCII commands we can use
 *  terminals and limited inputs to control flow. With binary this isn't
 *  as easy.
 * OLD SCHOOL
 *  Commands start with 3 letters white space and a set of optional numbers
 *  terminated by a semicolon. Spaces, carriage returns,
 *  indeed all non-printables are ignored.
 * NEW PATTERN
 *  Command pattern is not simple as some require additional information.
 *  problem with binary communications is that it is hard to know when a
 *  communication is done except by counting characters.  Some command
 *  predicates do not have parameters while others do and the parameters (in
 *  binary might look like commands so we have to be careful.)
 **/
void
ShieldCommunication::collectCommand() {
   // Since this is embeded in a serialEvent ISR there has to be
   // at least one character.  Because we deal with character reception
   // asynchronously we need to set up a state machine to collect the
   // characters to assemble the command.
   while(Serial.available()) {
     char cc = (char)Serial.read();
     if ( cc & 0x80 ) { // if the high order bit is set then this is a predicate
       _paramCount = (int)cc & 0x03; // mask count for additional bytes
       _predicate = cc;
       _param[0] = 0;
       _param[1] = 0;
       _param[2] = 0;
       break;
     } else { // we are a data value
       if ( _paramCount<1 ) return; // sync error ignore extra data byte
       --_paramCount; // decrement count
       _param[_paramCount] = cc;
       } // ifelse
    } // while
}

/**
 * combine the command into a single unsigned long
 * data parameters are 7 bits so this is a useful tool
 **/
unsigned long
ShieldCommunication::getParameter() {
  return (_param[2]<<14) + (_param[1]<<7) + _param[0];
}

/**
 * extract a single parameter
 **/
char
ShieldCommunication::getParameter(int i) {
  if ( i<1 || i>3 ) return 0; // safety first
  return _param[i-1];
}

/**
 * Flag the command as having been handled successfully and are returning to wait
 **/
void
ShieldCommunication::commandSuccessful() {
   Serial << "!"; //(char)0x06; //ASCII ACK
//   Serial.write((char)0x06);
   _paramCount=-1; // Ready to compile a new command.
}

/**
 * Flag the command as being bad and return to getting a new command
 **/
void
ShieldCommunication::badCommand() {
    Serial << "?"; // (char)0x15; //ASCII NAK
    _paramCount=-1; // Ready to compile a new command.
}

/**
 * Format and send current state
 **/
void
ShieldCommunication::sendStatus(char state) {
   Serial << "Status: " << state;
   Serial << " -cmd: " << _predicate << " (0x" << _HEX(_predicate) << ")";
   switch ( _predicate&0x03 ) {
      case 2:  Serial << " p2: " << _DEC(_param[1]) << ", 0x" << _HEX(_param[1]) << ", 0b" << _BIN(_param[1]);
      case 1:  Serial << " p1: " << _DEC(_param[0]) << ", 0x" << _HEX(_param[0]) << ", 0b" << _BIN(_param[0]);
   }
   Serial << endl;
}

/**
 * Format and send current state
 **/
void
ShieldCommunication::sendStatus(const char* report) {
   Serial << report; // send a string that is formatted by someone else.
   Serial << endl;
}

/**
 * Send data out through the serial port in a binary fashion
 * a data blob in this environment is organized in the following way:
      +------+------+------+------+------+------+------+------+
 byte |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
      +------+------+------+------+------+------+------+------+
      | 0xAA |  seq#  | data  |src|  microseconds since SYNC  |
 bits |  8   |   11   |  10   | 3 |            32             |
      +------+------+------+------+------+------+------+------+
 *
 **/
void
ShieldCommunication::sendDataBlob(int index, unsigned long clktime, int raw, int channel) {
  /**
  // The arduino does funny things with the ordering of these values.
  union dataBlob {
     uint8_t bytes[8];
     struct {
       uint8_t id : 8;
       uint16_t seqNo : 11;
       uint16_t rawData : 10;
       uint8_t srcID : 3;
       uint32_t timestamp : 32;
     } parts;
   } db;
   db.parts.id = 0xAA;
   db.parts.seqNo = (uint16_t)(index & 0x7FF);
   db.parts.rawData = (uint16_t)(raw & 0x1FF);
   db.parts.srcID = (uint8_t)(channel & 0x7);
   db.parts.timestamp = (uint32_t)clktime;
  **/
  index = index % 2048;  // wrap the count to 11 bits
  // The arduino is little endian. elaborate experimments with unions and bit
  // field structures cannot adjust for the funny way the data gets ordered.
  // This is the only sure fire way to avoid the whole gulliver's travels thing
  char dataBytes[8] = {
      (char) 0xAA, // flag
      (char) (( (uint16_t)index) >> 3),  // top 8 bytes of the index
      (char) (( (0x07 & (uint16_t)index) << 5) + ((uint16_t)raw>>5)),
      (char) (( (0x1F & (uint16_t)raw) << 3) + (0x7 & channel)),
      (char) (0xFF & (clktime>>24)),  // the time data is straight forward
      (char) (0xFF & (clktime>>16)),
      (char) (0xFF & (clktime>>8)),
      (char) (0xFF & clktime)
    };

  for( int i=0; i<8; i++) {
    Serial.write( dataBytes[i] );
//    Serial << " " << _BIN(db.bytes[i]);
  }
//  Serial << endl;
}

/**
 * Send a string (always starts with a space and ends with endl)
 **/
void
ShieldCommunication::sendString( String msg ) {
  Serial << " " << msg << endl;
}
