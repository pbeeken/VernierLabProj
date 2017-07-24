/***
 * Test Platform for Vernier Interface Shield
 * This suite is a simple echo so we can make sure the Shield
 * is receiving and decoding the messages correctly. There is
 * is a corresponding python file that runs the suite.
 ***/

#include <ShieldCommunication.h>
#include <Streaming.h>
#include <VernierBlinker.h>

VernierBlinker theLED;
ShieldCommunication cmd;

/**
 * Setup the Arduino before we enter the endless loop
 */
void setup() {
  Serial.begin(115200);

  // blink 5 times to indicate we are ready.
  for (int i = 0; i < 6; i++) {
    delay(100);
    theLED.turnOn();
    delay(100);
    theLED.turnOff();
  }
  // I have decided not to do this. Use successful
  // HALT command to test for readiness
//  Serial << "+@+"; // Ready to rock
}

/**
 * Run the loop repeatedly
 */
void loop() {

  if (cmd.isCommandComplete()) {
     // We are ready to process a command
     switch( cmd.getCommand() ) {
        case STATUS:  cmd.commandSuccessful();
                      Serial << "STATUS" <<endl;
                      cmd.sendStatus( 'Q' );
                      break;

       case TRIGMODE: cmd.commandSuccessful();
                      Serial << "TRIGMODE" <<endl;
                      cmd.sendStatus( 'S' );
                      break;
       case DIGMDE1:  cmd.commandSuccessful();
                        Serial << "DIGMDE1" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case DIGMDE2:  cmd.commandSuccessful();
                        Serial << "DIGMDE2" <<endl;
                        cmd.sendStatus( 'H' );
			             break;

       case ANAMDE1:  cmd.commandSuccessful();
                        Serial << "ANAMDE1" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case ANAMDE2:  cmd.commandSuccessful();
                        Serial << "ANAMDE2" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case ANAMDEB:  cmd.commandSuccessful();
                        Serial << "ANAMDEB" <<endl;
                        cmd.sendStatus( 'H' );
			             break;

       case IMMEDA1:  cmd.commandSuccessful();
                        Serial << "IMMEDA1" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case IMMEDA2:  cmd.commandSuccessful();
                        Serial << "IMMEDA2" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case IMMEDD1:  cmd.commandSuccessful();
                        Serial << "IMMEDD1" <<endl;
                        cmd.sendStatus( 'H' );
			             break;
       case IMMEDD2:  cmd.commandSuccessful();
                        Serial << "IMMEDD2" <<endl;
                        cmd.sendStatus( 'H' );
			             break;

      case HALT:     cmd.commandSuccessful();
                     Serial << "HALT" << endl;
                     cmd.sendStatus( 'I' );
                     break;
      case GO:       cmd.commandSuccessful();
                     Serial << "GO" << endl;
                     cmd.sendStatus( 'I' );
                     break;
      case BTN:      cmd.commandSuccessful();
                     Serial << "BTN" << endl;
                     cmd.sendStatus( 'I' );
                     break;

       default:      cmd.badCommand();
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
   if( cmd.isReadyToBuild() )
      cmd.collectCommand();

}
