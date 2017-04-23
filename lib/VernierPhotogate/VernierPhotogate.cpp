/****************************************************************
VernierPhotogate
	This is the superclass designated to serve as the model for
	all other digital sensors. It is unlikely anyone would use this
	class directly.  The way to use this object is through the
	subclass associated with the specific sensor that needs to
	be used.

	Tested and developed in Platformio 3.1.0
	PBeeken ByramHills High School 9.1.2016

	11 Nov 2016- P. Beeken, Byram Hils High School
****************************************************************/
#include <Arduino.h>
#include <VernierPhotogate.h>


/** Constructor
 *    setup button channel and initialize vars.
 */
VernierPhotogate::VernierPhotogate( int channel ) {
  _channel = channel;
//  _start_ms = 0L;
  _start_us = 0L;
}

/** readDigitalData
 *
 */
// DigitalReading
// VernierPhotogate::readGate() {
//   DigitalReading rv;
//   rv.timestamp_us = micros()-_start_us;
// //  rv.timestamp_ms = millis()-_start_ms;
//   rv.state = digitalRead(_channel);
//   return rv;
// }

/** read the current state of the gate
 *    true is an open gate, false is a blocked gate
 */
bool
VernierPhotogate::isGateClear() {
  return digitalRead(_channel);
}

/** reset the timer starting now!
 *    resets the internal values for subtracting from any subsequent times
 *    somewhere in the future maybe this can be used to reset the universal
 *    clock.
 */
void
VernierPhotogate::resetClock() {
  _start_us=micros();
//  _start_ms=millis();
}

/** time the block (or clear) of a gate.
 *    This routine times the transition from Block to Block until
 *    object clears gate. If gate is blocked already it will wait
 *    until it clears before beginning the timer again.
 */
bool
VernierPhotogate::gatherTimes( unsigned long timeout_us, int n, unsigned long *times ) {
   bool currState = isGateClear();
   // we wait for first transition
   unsigned long beg = micros();
   while ( isGateClear()==currState ) { // while it remains unchanged
      if ( (micros()-beg) > timeout_us ) return false;
   }

   // record subsequent transitions
   for( int i=0; i<n; i++ ) {
     currState = isGateClear();
     beg = micros();
     while ( isGateClear()==currState ) { // while it remains unchanged
        if ( (micros()-beg) > timeout_us ) return false;
     }
     times[i]=micros()-beg;
   }

  return true;
}

/** time the block (or clear) of a gate.
 *    This routine times the transition from Block to Block until
 *    object clears gate. If gate is blocked already it will wait
 *    until it clears before beginning the timer again.
 TODO: Rewrite this routine in terms of the more general one above.
 */
bool
VernierPhotogate::sequentialTiming( unsigned long timeout_us, SequentialTimes *times ) {
   unsigned long transitTimes[4];

  // start things off by waiting for first rise
   transitTimes[0] = micros();
   while ( isGateClear() ) {
      if ( (micros()-transitTimes[0]) > timeout_us ) return false;
   }
   transitTimes[1] = micros();

   // now go on to wait for next fall
    while ( !isGateClear() ) {
       if ( (micros()-transitTimes[1]) > timeout_us ) return false;
    }
    transitTimes[2] = micros();

    // wait for next rise
    while ( isGateClear() ) {
       if ( (micros()-transitTimes[2]) > timeout_us ) return false;
    }
    transitTimes[3] = micros();

    // finally go on to wait for next fall
    while ( !isGateClear() ) {
       if ( (micros()-transitTimes[3]) > timeout_us ) return false;
    }
    transitTimes[4] = micros();

  times->firstPulseWidth = transitTimes[2]-transitTimes[1];
  times->timeBetweenGates = transitTimes[3]-transitTimes[2];
  times->secondPulseWidth = transitTimes[4]-transitTimes[3];

 return true;

}

/** waits for gate to reach requested state
 *  This would be called right after the above
 */
unsigned long
VernierPhotogate::timeInGate( int trigger, unsigned long timeout_us ) {

  // deal with openning or closing
  switch( trigger ) {
      case BLOCKED:  // Solid object passing through
        return pulseIn(_channel, LOW, timeout_us );
        break;
      case CLEARED: // hole passing through
        return pulseIn(_channel, HIGH, timeout_us );
        break;
      }  // switch

      return 0;
}

//
// unsigned long Timems = 0; //Time in ms
// unsigned long Timeus = 0; //Time in us
// unsigned long lastTimems = 0;
// int PhotogatePin =2; //
// int Photogate = HIGH;
// int status;
// int LEDpin =13;/// line for LED to turn on when photogate is blocked
//
// void setup()
//   {
//    Serial.begin(9600);           // set up Serial library at 9600 bps
//     pinMode(LEDpin, OUTPUT);
//     Serial.println("Vernier Format 2");
//     Serial.println("Photogate blocked times taken using Ardunio");
//     Serial.print("Time");
//     Serial.print("\t"); //tab character
//     Serial.println ("Time"); //change to match sensor
//     Serial.print("ms");
//     Serial.print("\t"); // tab character
//     Serial.println ("microseconds");
//   };// end of setup
//
//
//
//
// // void loop ()
// // {
// //   Photogate = digitalRead(PhotogatePin);//low when blocked
// //    if (Photogate == LOW)
// //    {
// //     digitalWrite(LEDpin, HIGH);// turn on LED
// //         if (status == HIGH)
// //           {
// //           Serial.print(Photogate);
// //           Timems = millis() ;
// //           Timeus = micros() ;
// //           Serial.print(Timems);
// //           Serial.print("\t"); //tab character
// //           Serial.print(Timeus);
// //           Serial.print("\t"); //tab character
// //           Serial.println( Timems-lastTimems );
// //           lastTimems = Timems;
// //            }
// //    }
// //    else digitalWrite(LEDpin, LOW);// turn on LED
// //    status = Photogate;
// //  } ;// end of loop
//
// void loop()
// {
//   bool isClear = digitalRead( PhotogatePin ) == HIGH;
//   if ( isClear ) {
//     unsigned long timeInGate = pulseIn(PhotogatePin, LOW, 1000000 ); // don't wait more than 1 sec
//     if ( timeInGate>0 ) {
//       Serial.println( timeInGate );
//     } else {
//       Serial.println( "too long." );
//     }
//
//
//   }
//
// }
