/***
 * Test Platform for Vernier Interface Shield
 ***/

#include <Streaming.h>
#include <VernierBlinker.h>
#include <ShieldCommunication.h>

VernierBlinker      theLED;
ShieldCommunication cmd;

void setup()
{
   Serial.begin(115200);

   for( int i=0; i<6; i++) {
      delay(100);
      theLED.turnOn();
      delay(100);
      theLED.turnOff();
   }
   Serial << "BEG;" << endl;
}


void loop()
{
//   cmd.CollectCommand();

   if (cmd.isCommandComplete())
   {
      if (cmd.isCommand("LDA")&&cmd.getParameter()==1)
         theLED.turnOn();
      if (cmd.isCommand("LDA")&&cmd.getParameter()==0)
         theLED.turnOff();
      if (cmd.isCommand("LDA")&&cmd.getParameter()>1)
         theLED.blinkFor(cmd.getParameter());
      Serial << cmd.getCommand() << ".." << cmd.getParameter() << ";" << endl;
      cmd.commandComplete();           // we are done carrying out orders
   }
}

void serialEvent()
{
   cmd.CollectCommand();
}
