/****************************************************************
*  ShieldControl
*  This encapsulates the control and state engine for the Arduino
*  management of the Vernier Shield. There should be only one
*  instance of this object which is the data acquisition tool
*  Tested and developed in Platformio 3.1.0
*  PBeeken ByramHills High School 7.12.2017
*
*  12 Jul 2017- begin development of a binary communication protocol
*  ShieldControl was dropped in favor of VernierShieldCommunication which implements
*     a binary communication protocol and fully operational firmware with control by
*     a connected python app.
****************************************************************/

#include <ShieldControl.h>
#include <Streaming.h>



ShieldControl::ShieldControl() {
   setState( CLEAR_ALL ); // booting
}

void
ShieldControl::sendStatus(int maj_rev, int min_rev) {
   Serial << "Revision: " << maj_rev << "." << min_rev <<endl;
}
