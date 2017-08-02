/***
 * Test Platform for Vernier Interface Shield
 ***/

#include <ShieldCommunication.h>
#include <ShieldControl.h>

#include <Streaming.h>

#include <VernierDigitalSensor.h>
#include <VernierAnalogSensor.h>
#include <VernierBlinker.h>

VernierBlinker theLED;
ShieldCommunication comm;
ShieldControl shield;
VernierDigitalSensor dig1( VernierDigitalSensor::BTD01);
VernierDigitalSensor dig2( VernierDigitalSensor::BTD02);
VernierAnalogSensor ana105( VernierAnalogSensor::BTA01_5V);
VernierAnalogSensor ana110( VernierAnalogSensor::BTA01_10V);
VernierAnalogSensor ana205( VernierAnalogSensor::BTA02_5V);
VernierAnalogSensor ana210( VernierAnalogSensor::BTA02_10V);



// This will be replaced by the function for taking data when the time
// comes.
bool (*pollDataPort)();
int trigMode = 0;
int dataCountLimit = 100;

/**
 * Setup the Arduino before we enter the endless loop
 */
void setup() {
  Serial.begin(115200);
  pollDataPort = 0L;  // no data taking
  theLED.blinkFor(2);
}

const char msg[] = "Hello World";



/**
 * Run the loop repeatedly
 */
void loop() {

  // if this pointer is <>0 then we can safely assume it is
  // meant to gather data.
  if ( pollDataPort ) {
     pollDataPort();
  }

  // We enter this piece if a command is complete.
  if (comm.isCommandComplete()) {
     // We are ready to process a command
     switch( comm.getCommand() ) {
        case STATUS:  comm.commandSuccessful();
                      comm.sendStatus( msg );
                      shield.sendStatus();
                      break;

       // Setup Commands
       case TRIGMODE: comm.commandSuccessful();
                      // we get two values from this command
                      Serial << "TRIGMODE" <<endl;
                      comm.sendStatus( 'S' );
                      break;

       // Initiate data taking modes.
       case DIGMDE1:  comm.commandSuccessful();
                      dig1.initTrigger(comm.getParamOne());
			             break;
       case DIGMDE2:  comm.commandSuccessful();
                      dig2.initTrigger(comm.getParamOne());
			             break;
       case ANAMDE15:  comm.commandSuccessful();
                      ana105.setInterval(0x0F & comm.getParamOne());
			             break;
       case ANAMDE25:  comm.commandSuccessful();
                      ana205.setInterval(0x0F & comm.getParamOne());
			             break;
       case ANAMDE110:  comm.commandSuccessful();
                      ana110.setInterval(0x0F & comm.getParamOne());
			             break;
       case ANAMDE210:  comm.commandSuccessful();
                      ana210.setInterval(0x0F & comm.getParamOne());
			             break;

       // Commands that demand an immediate response
       case IMMEDA105: comm.commandSuccessful();
                       comm.sendDataBlob( micros(), ana105.readPort(), ANA105 );
                       break;
       case IMMEDA205: comm.commandSuccessful();
                       comm.sendDataBlob( micros(), ana205.readPort(), ANA205 );
			              break;
       case IMMEDA110: comm.commandSuccessful();
                        comm.sendDataBlob( micros(), ana110.readPort(), ANA110 );
                        break;
       case IMMEDA210: comm.commandSuccessful();
                       comm.sendDataBlob( micros(), ana210.readPort(), ANA210 );
   			              break;
       case IMMEDD1:  comm.commandSuccessful();
                     comm.sendDataBlob( micros(), dig1.readPort(), DIG1 );
			             break;
       case IMMEDD2:  comm.commandSuccessful();
                     comm.sendDataBlob( micros(), dig2.readPort(), DIG2 );
			             break;

      // State change commands
      case HALT:     comm.commandSuccessful();
                     Serial << "HALT" << endl;
                     comm.sendStatus( 'I' );
                     break;
      case GO:       comm.commandSuccessful();
                     Serial << "GO" << endl;
                     comm.sendStatus( 'I' );
                     break;
      case BTN:      comm.commandSuccessful();
                     Serial << "BTN" << endl;
                     comm.sendStatus( 'I' );
                     break;

       default:      comm.badCommand();
                     break;
     }

  }
}

/**
   Like setup() and loop() this is a predeclared function that is triggered
   whenever there is a character in the serial port. You can put what ever
   you want in this routine but it should concern itself with gathering
   characters in the small buffer.  It is run right after the loop() stub
**/
void serialEvent() {
   if( comm.isReadyToBuild() )
      comm.collectCommand();

}
