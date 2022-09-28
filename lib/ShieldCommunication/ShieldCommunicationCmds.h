/**
 * Command messages. These should be shared with the client program in order to
 *   link the commands that are sent out. Commands are 8 bits which means the high
 *   bit must be set. We also requre that the low two bits be available to indicate
 *   the number of data bytes to follow.  There can be up to 3 7bit (midi style)
 *   data bits to follow the intial command. We are thus limited in the number of
 *   available command bytes.
 **/

namespace CMDS {

  const char HALT    =0x80;          // 0b10000000  128   🢂 stop all activity and return to READY
  const char ARM     =0x84 | 0x01;   // 0b10000100  132   🢂 begin data acquisition process
  // parameter sets which ports to watch:
  // bit 0-1 analog channel 1 0: ignore, 1: ±6V signal, 2: ±10V signal, 3: both (non-vernier probe)
  // bit 2-4 analog channel 2 0: ignore, 1: ±6V signal, 2: ±10V signal, 3: both (non-vernier probe)
  // bit 5   digital channel 1 0: ignore, 1, use
  // bit 6   digital channel 1 0: ignore, 1, use
  // in this way it is possible to get results from either or both ports.
  // The vernier has two analog channels but each channel is connected to two different
  // ports that scale the inputs differently.  One port is scaled to ±6V per 10bits
  // while the other is scaled to ±10.  Standard Vernier probes are configured to use
  // one or the other but not both. The software can handle both in the event a
  // customized probe requires this.

  // depending on the conditions set with the mode command the arduino may start sending
  // data blobs right away or may delay based on trigger conditions. If the command is valid
  // and the data acquisition conditions set by the mode commands are valid then an ACK will
  // be sent by the Arduino otherwise a NACK will be sent.
  const char BLINKLED=0xA8 | 0x01;   // 0b10101000  168   🢂 blink led n times with period k
  // param 1: high nibble is # blinks 1-7, low nibble<<6 is period

  // These are immediate commands that solicit responses almost right away
  const char IMM_DIG1 =0x8C;          // 0b10001100  140   🢂 read dig port 1
  const char IMM_DIG2 =0x90;          // 0b10010000  144   🢂 read dig port 2
  const char IMM_AN051=0x94;          // 0b10010100  148   🢂 read analog  ±6V port 1
  const char IMM_AN101=0x98;          // 0b10011000  152   🢂 read analog ±10V port 1
  const char IMM_AN052=0x9C;          // 0b10011100  156   🢂 read analog  ±6V port 2
  const char IMM_AN102=0xA0;          // 0b10100000  160   🢂 read analog ±10V port 2
  const char IMM_BUTSTATE=0xA4;       // 0b10100100  164   🢂 get button press
  // all of the above commands will return a single datablob with seq# = 0

  // These are setup commands that set the conditions of the shield
  const char MDE_SYNC     =0xD0;     // 0b11010000  208   🢂 sync the clocks
  // synchronize the clocks across all the channels.

  const char MDE_ASAMPTIME =0xAC | 0x01;   // 0b10101100  172   🢂 set sample rate
  // sample rate determines the rate at which an analog sample is taken.
  // There are pre-determined sample rates that can be used.
  //    BUTTONPRESS = 0,  S_30s   = 15, S_10s   = 14, S_5s    = 13,
  //        S_2s    = 12, S_1Hz   = 11, S_5Hz   = 10, S_10Hz  = 9, --100 msec/sample
  //        S_20Hz  = 8,  S_40Hz  = 7,  S_50Hz  = 6,  S_100Hz = 5,
  //        S_200Hz = 4,  S_500Hz = 3,  S_1kHz  = 2,  FASTEST = 1, --1 msec, practically: 1.5 msec per sample
  // param: (int) (14bits) which sets the sampling interval [10Hz default]

  const char MDE_ASTOP =0xB0 | 0x02;   // 0b10110000  176   🢂 set stop condition
  // set the conditions for which sampling stops and Arduino returns to READY
  // parameter is uint16 (14bits)
  // param: 0 then datablobs will return forever until HALT is received [default]
  // param: two 7bit values are combined to give the count.

  const char MDE_ATRIG =0xB4 | 0x02;   // 0b10110100  180   🢂 set trigger condition
  // set the conditions for which sampling actually starts. parameter is uint16
  // high 2 bits the trigger type 0: immediate, 1: button press, 2: rising above threshhold on port
  //                              3: falling below threshhold on port
  // next 2 bits is the port to control 1: Ch1 or 2: Ch2
  // lowest 10 bits is threshhold for analog values,

  const char MDE_DTRIG =0xB8 | 0x01;   // 0b10111000  184   🢂 set the digital transition
  // high nibble of param: channel 1 settings: 1: L2H, 2: H2L, 3: ANY transition
  // low nibble of param: channel 2 settings: L2H, 2: H2L, 3: ANY transition
  // digital ports trigger when something happens and send a datablob at the
  // moment something the conditions warrant. Since all data has a signature as
  // to where it came from you can easily sort the results with the client. N.B. with
  // vernier digital gates, for example. It is possible to chain multiple gates together
  // on one port. With this configuration there is no way to distinguish which gate
  // triggered the event.

  // Status requests. Can be used to see if the Shield has been set up correctly or
  // just interrogate the firmware.
  const char ST_ANALOG =0xC8 | 0x01;   // 0b11001000  200   🢂 Status of AnalogPorts
  // parameter is simply the 1 indexed bit position of SOURCES port index
  const char ST_VERS   =0xC8;          // 0b11001000  200   🢂 version info

  /*** following is for future expansion
     const char STASTOP=0xC4;      // 0b11000100  196   🢂 stop status
     const char STASTATE=0xCC;     // 0b11001100  204   🢂 state
     const char NOP=0x88;          // 0b10001000  136   🢂 not used currently
     const char xxx=0xC4;          // 0b11000100  196
     const char xxx=0xD4;          // 0b11010100  212
     const char xxx=0xD8;          // 0b11011000  216
     const char xxx=0xDC;          // 0b11011100  220
     const char xxx=0xE0;          // 0b11100000  224
     const char xxx=0xE4;          // 0b11100100  228
     const char xxx=0xE8;          // 0b11101000  232
     const char xxx=0xEC;          // 0b11101100  236
     const char xxx=0xF0;          // 0b11110000  240
     const char xxx=0xF4;          // 0b11110100  244
     const char xxx=0xF8;          // 0b11111000  248
     const char xxx=0xFC;          // 0b11111100  252
   ***/
};

// Flags that mark the data sources in the high nibble of the data blob.
namespace SOURCES {
  const int ANA105 = 0x3;
  const int ANA205 = 0x4;
  const int ANA110 = 0x5;
  const int ANA210 = 0x6;
  const int DIG1 = 0x1;
  const int DIG2 = 0x2;
  const int BTN = 0x7;
};
