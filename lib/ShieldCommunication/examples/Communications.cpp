/***
 * Test Platform for Vernier Interface Shield
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
  Serial << "+@+"; // Ready to rock
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
                      cmd.sendStatus( 'H' );
                      break;

       case TRIGMODE: cmd.commandSuccessful();
                      Serial << "TRIGMODE" <<endl;
                      cmd.sendStatus( 'H' );
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
			             break;
       case IMMEDA2:  cmd.commandSuccessful();
                        Serial << "IMMEDA2" <<endl;
			             break;
       case IMMEDD1:  cmd.commandSuccessful();
                        Serial << "IMMEDD1" <<endl;
			             break;
       case IMMEDD2:  cmd.commandSuccessful();
                        Serial << "IMMEDD2" <<endl;
			             break;

      case HALT:     cmd.commandSuccessful();
                     Serial << "HALT" << endl;
                     break;
      case GO:       cmd.commandSuccessful();
                     Serial << "GO" << endl;
                     break;
      case BTN:      cmd.commandSuccessful();
                     Serial << "BTN" << endl;
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
   characters in the small buffer.
**/
void serialEvent() {
   if( cmd.isReadyToReceive() )
      cmd.collectCommand();

}
