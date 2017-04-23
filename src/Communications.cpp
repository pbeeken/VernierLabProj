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

   theLED.turnOn();
   delay(1000);      // 1 second
   theLED.turnOff();
}


void loop()
{
   if (cmd.isCommandComplete())
   {
      if (cmd.isCommand("ON"))
         theLED.turnOn();
      if (cmd.isCommand("OFF"))
         theLED.turnOff();
      if (cmd.isCommand("BLK"))
         theLED.blinkFor(cmd.getParameter());
      Serial << ".." << cmd.getParameter() << ".." << endl;
      cmd.commandComplete();           // we are done carrying out orders
   }
}

void serialEvent()
{
   cmd.CollectCommand();
}
