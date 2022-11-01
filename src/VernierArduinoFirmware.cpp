/***
 * Platform for Vernier Interface Shield as a black box a.k.a. Firmware 
 * 
 * First a shout out to the folks at SparkFun for their nice shield
 * and the code they released.  I couldn't have embarked on this 
 * project without original code to get me started on this.
 * 
 * This is a labor of love.  I make no profit off this software and
 * it is definitely a work in progress.  As I discover bugs I try to
 * fix them but I am a physics teacher first and a coder second.
 * 
 * The VernierLab Software can be used in at least two different ways:
 *   1. Use the collection of objects with Arduino IDE (or better still, vscode and Platformio)
 *      to directly modify the arduino as a data acquisition system for a specific purpose.
 *   2. Build a black box 'firmware' application that allows the Arduino and shield to be
 *      controlled from another computer as the "Labquest" boxes are using their proprietary firmware 
 *      loaded on whatever microcontroller it runs on.  This file is the source code for just this kind 
 *      of application. The idea is to set this up, deploy it across a class set of arduino unos with
 *      the vernier hats. When sporting a nice 3D printed case these boxes can be controlled from python code
 *      snippets within a Jupyter Notebook which not only allows data acquisition but data analysis as
 *      well.
 * 
 * M.I.T. license ensues, feel free to copy and modify anything here but
 * please attribute.
 * 
 *  Tested and developed in PlatformIO for vsCode 3.1.0
 *  PBeeken ByramHills High School 7.12.2017
 *  3/20/2020 Some cosmetic changes
 *  ----- Long interval CoVid eneded labs for a while so this project went on a back burner.
 *  9/22/2022 Project got put on a back burner while I developed curriculum using Jupyter Notebook
 *              I realzed that this device would fit perfectly with the labs we run.  This 'firmware'
 *              runs with just the bare-bones analog and digital input. Control is meant to be done
 *              using JN running on the same machine as the interface. Data is pulled from the device
 *              and calibrations based on the device attached to the BTA and BTD connectors are controled
 *              python.
 * 10/28/2022 Extensive testing and tweaking have verified that this idea works and pretty reliably as well.
 * 
 ***/

#include <ShieldCommunication.h>
//#include <ShieldControl.h>

#include <Streaming.h>

#include <VernierDigitalSensor.h>
#include <VernierAnalogSensor.h>
#include <VernierBlinker.h>

/**
 * ShieldControl shield objects
 *   we just load the basic objects. Data Acquisition (DA) objects return their raw
 *   digital values. The python libraries that connect to this code on the arduino
 *   through the USB port will hold the calibration and conversion routines.  The 
 *   philosophy of this whole framework is the that students should know what they
 *   are connecting to which port and choose the correct calibration routines.
 *  
 **/
VernierBlinker theLED;
VernierButton theBtn;

// DA Sensor Objects
VernierDigitalSensor dig1(VernierDigitalSensor::BTD01);
VernierDigitalSensor dig2(VernierDigitalSensor::BTD02);
VernierAnalogSensor ana105(VernierAnalogSensor::BTA01_5V);
VernierAnalogSensor ana110(VernierAnalogSensor::BTA01_10V);
VernierAnalogSensor ana205(VernierAnalogSensor::BTA02_5V);
VernierAnalogSensor ana210(VernierAnalogSensor::BTA02_10V);

ShieldCommunication comm;

const char BOOT_MSG[] = "*HELLO*";
const char MAJOR_REV[] = "1";
const char MINOR_REV[] = "03";

/**
 * Synchronize clocks.  This makes sure that the inputs share 
 * the same starting point. Is this perfect? No. but it is 
 * close enough.  We also zero the data count while we are at it.
 */
uint32_t dataCount = 0;

void syncClocks() {
        dataCount = 0L;
        unsigned long matchClocks = micros();
        dig1.sync(matchClocks);
        dig2.sync(matchClocks);
        ana105.sync(matchClocks);
        ana110.sync(matchClocks);
        ana205.sync(matchClocks);
        ana210.sync(matchClocks);
        theBtn.sync(matchClocks);
}


/**
 * Setup the Arduino before we enter the endless loop
 */
void setup() {
        Serial.begin(4*115200);  // We are talking over USB. 115200  We may be able to push this up to 1E6
        theLED.setBlinkPeriod(200);
        theLED.blinkFor(3);
        syncClocks();
        Serial << BOOT_MSG << " ver:" << MAJOR_REV << "." << MINOR_REV << endl; // send boot message
}

// this turns the SOURCES index into a bit to test
#define SRC_BITLOC(src) (1<<(src-1))

/**
 * Run the loop repeatedly
 */
void loop() {

        // Poll the ports first. Many of these calls take next to no time if the port is flagged as HALTed
        if( ana105.pollPort() ) {  // Only takes <~4μS if off
                comm.sendDataBlob(ana105.getCount(), ana105.getAbsTime(), ana105.getLastRead(), SOURCES::ANA105);
        }
        if( ana205.pollPort() ) {  // Only takes <~4μS
                comm.sendDataBlob(ana205.getCount(), ana205.getAbsTime(), ana205.getLastRead(), SOURCES::ANA205);
        }
        if( ana110.pollPort() ) {  // Only takes <~4μS
                comm.sendDataBlob(ana110.getCount(), ana110.getAbsTime(), ana110.getLastRead(), SOURCES::ANA110);
        }
        if( ana210.pollPort() ) {  // Only takes <~4μS
                comm.sendDataBlob(ana210.getCount(), ana210.getAbsTime(), ana210.getLastRead(), SOURCES::ANA210);
        }
        if( dig1.pollPort() ) {  // Only takes <~4μS
                comm.sendDataBlob(dig1.getCount(), dig1.getDeltaTime(), dig1.getTransitionType(), SOURCES::DIG1);
        }
        if( dig2.pollPort() ) {  // Only takes <~4μS
                comm.sendDataBlob(dig2.getCount(), dig2.getDeltaTime(), dig2.getTransitionType(), SOURCES::DIG2);
        }

}


/**
   Like setup() and loop() this is a predeclared function that is triggered
   whenever there is a character in the serial port. You can put whatever
   you want in this routine but it should concern itself with processing
   the characters in the buffer.  It is run right after the loop() stub
 **/
void serialEvent() {
        // see if we have a character waiting.
        if( comm.isReadyToBuild() )
                comm.collectCommand();

        // See if we have a completed command.
        // We enter this piece if a command is complete and parameters are all in place
        if( comm.isCommandComplete() ) {
                switch( comm.getCommand() ) {
                // use the SOURCES index to mark the bits to set.
                // arm the channels to get ready for data acquisition
                case CMDS::ARM:
                        if ( comm.getParameter(1) & bit(SOURCES::ANA105-1) )  ana105.armPort();
                        if ( comm.getParameter(1) & bit(SOURCES::ANA205-1) )  ana205.armPort();
                        if ( comm.getParameter(1) & bit(SOURCES::ANA110-1) )  ana110.armPort();
                        if ( comm.getParameter(1) & bit(SOURCES::ANA210-1) )  ana210.armPort();
                        if ( comm.getParameter(1) & bit(SOURCES::DIG1-1) )    dig1.armPort();
                        if ( comm.getParameter(1) & bit(SOURCES::DIG2-1) )    dig2.armPort();
                        comm.commandSuccessful();
                        break;

                // stop the data ports
                case CMDS::HALT:
                        comm.commandSuccessful();
                        ana105.haltPort();
                        ana110.haltPort();
                        ana205.haltPort();
                        ana210.haltPort();
                        dig1.haltPort();
                        dig2.haltPort();
                        break;

                // blink the led based on parameters
                case CMDS::BLINKLED: {
                        comm.commandSuccessful();
                        int blinks = (comm.getParameter() & 0xF0)>>4;
                        unsigned long timing = (unsigned long)(comm.getParameter() & 0x0F)<<7;
                        if (timing > 0) theLED.setBlinkPeriod(timing);
                        theLED.blinkFor(blinks);
                        }
                        break;

                // read the current button state
                case CMDS::IMM_BUTSTATE:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, theBtn.getCurrentTime(), (int)theBtn.buttonIsDown(), SOURCES::BTN );
                        break;

                // read the current digital gate 1
                case CMDS::IMM_DIG1:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, dig1.getCurrentTime(), (int)dig1.readPort(), SOURCES::DIG1 );
                        break;

                // read the current digital gate 2
                case CMDS::IMM_DIG2:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, dig2.getCurrentTime(), (int)dig2.readPort(), SOURCES::DIG2 );
                        break;

                // read the current analog 5V channel 1
                case CMDS::IMM_AN051:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, ana105.getCurrentTime(), ana105.readPort(), SOURCES::ANA105 );
                        break;

                // read the current analog 10V channel 1
                case CMDS::IMM_AN101:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, ana110.getCurrentTime(), ana110.readPort(), SOURCES::ANA110 );
                        break;

                // read the current analog 5V channel 2
                case CMDS::IMM_AN052:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, ana205.getCurrentTime(), ana205.readPort(), SOURCES::ANA205 );
                        break;

                // read the current analog 10V channel 2
                case CMDS::IMM_AN102:
                        comm.commandSuccessful();
                        comm.sendDataBlob( dataCount++, ana210.getCurrentTime(), ana210.readPort(), SOURCES::ANA210 );
                        break;

                // Synchronize the clocks
                case CMDS::MDE_SYNC:
                        comm.commandSuccessful();
                        syncClocks();
                        break;

                // Set the sample rate,
                // by default we use 10Hz (relevant to analog ports)
                // TODO: can consider allowing different sampling rates for the ADCs
                case CMDS::MDE_ASAMPTIME:
                        if ( comm.getParameter(1)<16 ) {
                                ana105.setSampleRate(comm.getParameter(1));
                                ana110.setSampleRate(comm.getParameter(1));
                                ana205.setSampleRate(comm.getParameter(1));
                                ana210.setSampleRate(comm.getParameter(1));
                                comm.commandSuccessful();
                        }
                        else
                                comm.badCommand();
                        break;

                // Set up the triggers,
                // set the conditions for which sampling actually starts. parameter is uint16 (2 7bit bytes)
                // high 2 bits the trigger type 0: immediate, 2: rising above threshhold on port
                //                              2: falling below threshhold on port, 3: button press
                // next 2 bits is the port to control 1: Ch1 or 2: Ch2
                // lowest 10 bits is threshhold for analog values,
                case CMDS::MDE_ATRIG: {
                                int type = (comm.getParameter()>>12)&0x3;
                                int chan = (comm.getParameter()>>10)&0x3;
                                if ( chan&0x1 ) {    // trigger conditions for channel 1
                                        ana105.setTrigger( type, comm.getParameter()&0x3FF );
                                        ana110.setTrigger( type, comm.getParameter()&0x3FF );
                                }
                                if ( chan&0x2 ) {    // trigger conditions for channel 2
                                        ana205.setTrigger( type, comm.getParameter()&0x3FF );
                                        ana210.setTrigger( type, comm.getParameter()&0x3FF );
                                }
                                comm.commandSuccessful();
                        }
                        break;

                // Set the stop condition,
                // set the conditions for which sampling stops and readings returns to HALT
                // parameter is simply the number of points.
                case CMDS::MDE_ASTOP: {
                                // bool useTime = (comm.getParameter(2) & 0x040) != 0; // 14th bit (#13 with zero offset) is time/flag count
                                int data = comm.getParameter() & 0x03FFF; // everything else is the data (time or count)
                                ana105.setStopCondition( data );
                                ana205.setStopCondition( data );
                                ana110.setStopCondition( data );
                                ana210.setStopCondition( data );
                                comm.commandSuccessful();
                        }
                        break;

                // Set the digital port signal transitions
                // high nibble of param: port 1 settings: 1: L2H, 2: H2L, 3: ANY transition
                // low nibble of param: port 2 settings: L2H, 2: H2L, 3: ANY transition
                // digital ports trigger when something happens and send a datablob at the
                // moment something the conditions warrant. Since all data has a signature as
                // to where it came from you can easily sort the results with the client.
                // N.B. with vernier photogates it is possible to chain multiple gates together
                // on one port. With this configuration there is no way to distinguish which gate
                // triggered the event.
                case CMDS::MDE_DTRIG:
                        dig1.setTrigger( comm.getParameter()&0xF );
                        dig2.setTrigger( comm.getParameter()>>4 );
                        comm.commandSuccessful();
                        break;

                // Status messages.
                case CMDS::ST_VERS: {
                                comm.commandSuccessful();
                                String msg("v:");
                                msg += MAJOR_REV;
                                msg += ".";
                                msg += MINOR_REV;
                                comm.sendString( msg.begin() );
                        }
                        break;

                case CMDS::ST_PORTS: { // report status
                                comm.commandSuccessful();
                                if ( comm.getParameter(1) & bit(SOURCES::ANA105-1) ) { // BTA01_5V
                                        String msg = ana105.getStatus("\"BTA01_5V\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::ANA205-1) ) { // BTA02_5V
                                        String msg = ana205.getStatus("\"BTA02_5V\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::ANA110-1) ) { // BTA01_10V
                                        String msg = ana110.getStatus("\"BTA01_10V\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::ANA210-1) ) { // BTA02_10V
                                        String msg = ana210.getStatus("\"BTA02_10V\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::DIG1-1) ) { // BTD01
                                        String msg = dig1.getStatus("\"BTD01\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::DIG2-1) ) { // BTD02
                                        String msg = dig2.getStatus("\"BTD02\":");
                                        comm.sendString( msg.begin() );
                                        }
                                if ( comm.getParameter(1) & bit(SOURCES::BTN-1) ) { // BTN
                                        String msg = "\"BTN\":";
                                        if (theBtn.buttonIsDown()) msg += "true";
                                        else                       msg += "false";
                                        comm.sendString( msg.begin() );
                                        }                        }
                        break;
                        // Set the digital port signal transitions

                }
        }

}
