"""
    This is an elementary first shot at developing a protocol for controlling an Vernier shield from an Arduino Uno
"""

import serial
import time
#cfg="/dev/ttyACM0  115200,8,N,1"

commands = { 'HALT':0x2C, 'GO':0x28, 'BTN':0x24,
             'DIGMDE1':0x31, 'DIGMDE2': 0x35,
             'ANAMDE1': 0x42, 'ANAMDE2': 0x46, 'ANAMDEB': 0x4A,
             'TRIGMODE': 0x52,
             'IMMEDA1': 0x60, 'IMMEDA2': 0x64, 'IMMEDD1': 0x70, 'IMMEDD2': 0x74,
             'STATUS': 0x50,
             }

class VernierCommunicate:
    """
    Create a communications toolkit for talking to our command protocol
    """
    serPort = 0

    # establish the virtual port through the USB port.
    def __init__(self):
        self.serPort = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)

    # send a command
    def sendCmd(self, pred, param1=0, param2=0):
        # send the predicate
        self.serPort.write(pred.to_bytes(1,byteorder='big'))
        if (pred&0b11)==1:  # 1 parameter to complete command
            self.serPort.write(param1.to_bytes(1,byteorder='big'))
        if (pred&0b11)==2:  # 2 parameters (send the 2nd one first)
            self.serPort.write(param2.to_bytes(1,byteorder='big'))
            self.serPort.write(param1.to_bytes(1,byteorder='big'))
        return self.getAck()

    # capture the handshake character right after we send the command
    def getAck(self):
        cc = self.serPort.read(1)
        if cc==b'!':
            return True
        if cc==b'?':
            return False
        else:
            raise Exception("Don't understand ack/nak response.",cc)

    # clear the buffer if something goes wrong.
    def clearBuffer(self):
        l = self.serPort.readline()
        while l!=b'':
            l = self.serPort.readline()

    # get a line of values
    def getNextLine(self):
        return self.serPort.readline().decode()

    # if we restarted out machine then this is the string that signals the Arduino is ready
    def waitForStart(self):
        try:
            for i in range(3):  # Take three shots at this.
                if self.sendCmd(commands['HALT']):
                    self.clearBuffer() # clear any other noise
                    return True
        except:
            return False

    # close the serial port so things can be cleaned up.
    def close(self):
        self.serPort.close()

def main():

    # Note that the first time we make this connection the Arduino sometimes makes
    ver = VernierCommunicate()

    try:
        if not ver.waitForStart():
                raise Exception( 'Arduino not responding.',0)
        print( 'begin...' )

        p1=1
        p2=31

        for acmd in commands:
            p1+=1
            p2+=5
            print( "Testing response to: {0} [0x{1:X}] p1: 0x{2:02X} p2: 0x{3:02X}".format(acmd,commands[acmd],p1,p2) )
            if ver.sendCmd(commands[acmd],param1=p1,param2=p2):
                l = ver.getNextLine()
                print(l.split('\r')[0])
                l = ver.getNextLine()
                print(l.split('\r')[0])
                print(" ----- ")


    except Exception as err:
        print( err.args )
        pass

    ver.close()





if __name__ == '__main__':
   main()