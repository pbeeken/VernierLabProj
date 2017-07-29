"""
    This is an elementary first shot at developing a protocol for controlling an Vernier shield from an Arduino Uno
"""

import serial
import time

"""
Basic definitions for commands and other common structures carried over from C code
"""
# noinspection SpellCheckingInspection
commands = {  # State changing commands
             'HALT': 0x2C, 'GO': 0x28, 'BTN': 0x24,
             # Mode changing commands (what are we reading...)
             'DIGMDE1': 0x31, 'DIGMDE2': 0x35,
             'ANAMDE1': 0x42, 'ANAMDE2': 0x46, 'ANAMDEB': 0x4A,
             # (... start and stop conditions
             'TRIGMODE': 0x52, 'STOPMODE': 0x56,
             # Interrogation
             'STATUS': 0x50,
             # Get immediate reads
             'IMMEDA105': 0x60, 'IMMEDA205': 0x64,
             'IMMEDA110': 0x68, 'IMMEDA210': 0x6A,
             'IMMEDD1': 0x70,   'IMMEDD2': 0x74,
             }

"""
Data sources (flag in high nibble of data field)
"""
sources = {'DIG1': 0x10,   'DIG2': 0x20,
           'ANA105': 0x30, 'ANA205': 0x40,
           'ANA110': 0x50, 'ANA210': 0x60,
           }

"""
Communicatoin object that talks to the Arduino Vernier Shield
"""


class VernierShieldCommunication:
    """
    Create a communications toolkit for talking to our Vernier shield
    """


    serPort = 0

    # establish the virtual port through the USB port.
    def __init__(self):
        self.serPort = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)
        time.sleep(3)

    # send a command
    def command(self, pred, param1=0, param2=0):
        # send the predicate
        self.serPort.write(pred.to_bytes(1, byteorder='big'))
        if (pred & 0b11) == 1:  # 1 parameter to complete command
            self.serPort.write(param1.to_bytes(1, byteorder='big'))
        if (pred & 0b11) == 2:  # 2 parameters (send the 2nd one first)
            self.serPort.write(param2.to_bytes(1, byteorder='big'))
            self.serPort.write(param1.to_bytes(1, byteorder='big'))
        return self.acknowledge()

    # capture the handshake character right after we send the command
    def acknowledge(self):
        cc = self.serPort.read(1)
        if cc == b'!':
            return True
        if cc == b'?':
            return False
        else:
            raise Exception("Don't understand ack/nak response.", cc)

    # clear the buffer if something goes wrong.
    def clearbuffer(self):
        l = self.serPort.readline()
        while l != b'':
            l = self.serPort.readline()

    # get a line of values
    def getbuffer(self):
        return self.serPort.readline().decode()

    # if we restarted out machine then this is the string that signals the Arduino is ready
    def waitforit(self):
        try:
            for i in range(3):  # Take three shots at this.
                if self.command(commands['HALT']):
                    self.clearbuffer()  # clear any other noise
                    return True
                time.sleep(1.0)  # snooze for a short while
        except:
            return False

    def datablob(self):
        raw = self.serPort.read(6)  # grab 6 bytes
        micro = (raw[0] << 24) + (raw[1] << 16) + (raw[2] << 8) + (raw[3])  # long microsecond time
        dsrc = (raw[4] & 0xF0)  # data source flag
        dval = ((raw[4] & 0x0F) << 8) + (raw[5])
        return micro, dval, dsrc

    # close the serial port so things can be cleaned up.
    def close(self):
        self.serPort.close()




"""
Main loop for testing the above object
"""


def main():

    # Note that the first time we make this connection the Arduino sometimes makes
    ver = VernierShieldCommunication()

    try:
        if not ver.waitforit():
                raise Exception('Arduino not responding.', 0)
        print('begin...')

        p1 = 0
        p2 = 0

        for acmd in ['IMMEDA105', 'IMMEDA205', 'IMMEDA110', 'IMMEDA210', 'IMMEDD1', 'IMMEDD2']:
            print("Testing response to: {0} [0x{1:X}] p1: 0x{2:02X} p2: 0x{3:02X}".format(acmd, commands[acmd], p1, p2))
            if ver.command(commands[acmd]):
                t, d, f = ver.datablob()
                print(" data time: {0} [{1}]".format(t, t/1000000.0))
                if d > 1:
                    print("      data: {0} [{1}]".format(d, d-512))
                else:
                    print("      data: {0} [{1}]".format(d, ['BLOCKED', 'OPEN'][d]))
                src = next(k for k, v in sources.items() if v == f)
                print("         f: {0} [{1}]".format(f, src))
                print(" ----- ")

    except Exception as err:
        print(err.args)
        pass

    ver.close()

if name == '__main__':
    main()
