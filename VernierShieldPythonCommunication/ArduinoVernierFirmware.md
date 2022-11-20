# Arduino-Vernier-Firmware

![Image of Vernier-Arduino Shield](VerSheImg)

This is an attempt at creating a firmware for controlling the [SparkFun Vernier Shield](SparkFun Vernier Shield).  This encapsulates a library of classes which can be used to control specific capabilities of the shield and intended for those who want to create a custom application to take data. This particular project is designed to be a more generic interface to both test the library and to provide ready access to high level routines.

## Why not Firmata?
I was always aware of [Firmata](Firmata) but never really played with it until I was about 1/2 way through building this library. I played with writing a control architecture based entirely on client side calls.  It does work but the best practical sampling times are on the order of 5-10Hz.  The theoretical limit of the Arduino itself is on the order of 10kHz. The overhead in communication is sufficiently high I am continuing with a framework better customized to data acquisition. Firmata is an excellent general toolkit. It's mission is to provide a way for high end clients (read:laptops and remote computers) to access the capabilities of connected microcontrolers. Like linux it attempts to be a general purpose firmware for many different microdevices.  The design point is to unify client communication.  In developing my firmware system I am aiming at a specific use case.

While what I am attempting here could fit into the same framework (I could clone and branch the Firmata repo) but it would require almost as much time to get a working version of this on my own.  That said, I learned a lot by looking at the general control structures and philosophy of the Firmata protocol.

## Philosophy
What I am going for is to provide a more open ended framework for automating measurements for school labs. Right now most students using electronic data acquisition tools rely on proprietary graphing and control software. While convenient, these tools insulate the students from the data gathering and analysis portions of the labs. In university research, working at IBM research, and now teaching high school physics I often have to devise automated tools for gathering large amounts of data which serves as the fuel for data analysis. The idea is to provide a foundation for setting up student directed tools for gathering data.

This idea is neither new nor is it for every class. Not all students are ready for this. But I can see a way where this can play a role in advanced quantitative science classes in secondary school, engineering courses, and laboratory sciences at universities. Once complete, for example, this could be used in a stand-alone python interpreter or incorporated as a library in a [Jupyter](Jupyter) framework (running a local kernel). Automated data gathering and scientific data analysis is now (and has been for a while) an essential skill along with 'graphing' and 'calculation' often explicitly taught in introductory classes.

## Protocol
Commands sent to the microcontroller are short and sweet. The commands are one to 4 bytes in length. The high bit of the first byte is always set which flags it as a 'command'. The first byte encodes the base command which and also encodes the additional number of bytes (0-3) to follow in the lower 2 bits. This leaves 32 possible commands. As with midi and Firmata the subsequent data bytes are only 7 bits with the high bit set low as a check to synchronize the data stream.
```
    +-+-+-+-+-+-+-+-+
bit |7|6|5|4|3|2|1|0|
    |1|   cmd   | # |   cmd: 5 bit command, #: no. bytes to follow
    +-+-+-+-+-+-+-+-+
```
The data bytes that might follow will only consist of 7 bytes. In this way, if a byte was dropped along the way, the Arduino will simply drop the string until a new command comes in.  The data that might follow is different for each command.  All commands are acknowledged with a one byte response: '!' or '?'.  '!' acknowledges that a command was received and validated (doesn't mean the Arduino can do it). A response of '?', for example, simply means the message it got didn't match any pattern it could understand.

Note that when this is all done, many of these handshaking details will be incorporated in the client software but I document it here for those who want to understand how this all works (and to be honest, keep it straight in my own head).

#### DataBlobs
Responses to commands (past the handshake) may come in two forms. Some commands may result in nothing (beyond the handshake, like 'RESET' or 'HALT'.) Others commands will elicit one or more blocks of data (called a DataBlob, thank you Google). DataBlobs are 8 bytes in length where the first byte is 0xAA. Some commands will deliver one data blob where others, like a free running sample, pumps out a rapid fire sequence. In those instances the opening byte of 0xAA will allow a means of synchronization in case something goes wrong in the transport. Methods like this limits the number of dropped values to one or two.

```
     +------+------+------+------+------+------+------+------+
byte |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |
     +------+------+------+------+------+------+------+------+
     | 0xAA |seq#  | data |src   |  microseconds since SYNC  |
     +------+------+------+------+------+------+------+------+
```
All DataBlobs are stamped with the microsecond timer value. A SYNC command synchronizes and zeros the time marker and sequence marker and values will increment until rollover. Rollover happens about every 56 minutes so there is plenty of time to accumulate short time information. If data is needed on a longer time scale then data can be gathered using the 'IMMED' commands and time data in the blob can be ignored. Data is, at most, 10 bits on the Arduino (for analog) so I use the top 11 bits track the sequence.  For immediate commands the seq# is always 0. For continuous measurements the seq# autoincrements with each sent packet. The first seq# in a stream sent after a SYNC command is 1. This is a way to know whether this Blob is all alone or one of a sequence. If the sequence number exceeds 2048 then the value will wrap around. If a terminal condition is set on the Ard/Shield then the final value sent will have this number set to 0 (flags a final blob). The next 10 bits is the raw ADC reading. The remaining 3 bit sequence identifies the source of the data.

#### Strings
The other elicited response can be a simple CR terminated string. This kind of response is intended to be delivered once and deliberately meant to be read easily in a terminal. Often they are simple status requests or version information or the startup message.  The first character of a response string is always a space.
```
     +------+--------+--------+
byte |   0  |   1  ... n      |
     +------+--------+--------+
     | 0x20 |  Hello World.\n |
     +------+--------+--------+
```

## _Example_
A typical sequence that elicits a response is a IMM_AN051. The command is an 'immediate' command in that the device tries to respond right away and return the value of a particular analog channel (note that the Vernier Shield actually uses two ports per channel, see the documentatoin on the Vernier Shield library).

 | Host Computer | dir | Arduino/Shield |
 |:-:|:-:|:-:|
 | *Send AREAD_10V_A1 Command* | ⟾ |  |
 | *good/bad* | ⟽ | _ack/nak_ |
 | *if good, get blob* | ⟽ | send DataBlob |


## Genreal References
* [atom editor](http://www.atom.io "Atom Hackable Editor")
* [Firmata]("https://www.arduino.cc/en/Reference/Firmata")
* [Platformio](http://platformio.org/ "Open source IoT IDE")
* [SparkFun Vernier Shield](https://github.com/sparkfun/Vernier_Interface_Shield)
* [Jupyter](http://jupyter.org/)
* [SparkFun Vernier Shield](https://www.sparkfun.com/products/12858) 
* [VerSheImg](https://cdn.sparkfun.com//assets/parts/9/7/2/4/12858-01.jpg)

