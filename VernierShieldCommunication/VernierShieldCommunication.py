from encodings import utf_8
import serial
import logging
import bitstring
import time
import json
from functools import reduce

class Commands:
    # If a command is valid and the data acquisition conditions set by the mode commands
    # are valid then an ACK will be sent by the Arduino otherwise a NACK will be sent.
    HALT = 0x80       # 0b10000000  128   ⇨ stop all activity and return to READY
    ARM = 0x84 | 0x01 # 0b10000100  132   ⇨ begin data acquisition process
    # depending on the conditions set with the mode command the arduino may start sending
    # data blobs right away or may delay based on trigger conditions.
    # resets all triggering and limiting parameters to their defaults.
    # bit 0-1 analog channel 1 0: ignore, 1: ±6V signal, 2: ±10V signal, 3: both (non-vernier probe)
    # bit 2-4 analog channel 2 0: ignore, 1: ±6V signal, 2: ±10V signal, 3: both (non-vernier probe)
    # bit 5   digital channel 1 0: ignore, 1, use
    # bit 6   digital channel 1 0: ignore, 1, use
    # in this way it is possible to get results from either or both ports.
    # The vernier has two analog channels but each channel is connected to two different
    # ports that scale the inputs differently.  One port is scaled to ±6V per 10bits
    # while the other is scaled to ±10.  Standard Vernier probes are configured to use
    # one or the other but not both. The software can handle both in the event a
    # customized probe requires this.
    BLINKLED = 0xA8 | 0x01  # 0b10101000  168   ⇨ blink led n times with period k
    # param 1: high nibble is # blinks 1-7, low nibble<<6 is period

    IMM_DIG1 = 0x8C   # 0b10001100  140   ⇨ read dig port 1
    IMM_DIG2 = 0x90   # 0b10010000  144   ⇨ read dig port 2
    IMM_AN051 = 0x94  # 0b10010100  148   ⇨ read analog  5V port 1
    IMM_AN101 = 0x98  # 0b10011000  152   ⇨ read analog 10V port 1
    IMM_AN052 = 0x9C  # 0b10011100  156   ⇨ read analog  5V port 2
    IMM_AN102 = 0xA0  # 0b10100000  160   ⇨ read analog 10V port 2
    # all of the above commands will return a single datablob with seq#': 0

    IMM_BUTSTATE = 0xA4  # 0b10100100  164   ⇨ get button press
    MDE_SYNC = 0xD0      # 0b11010000  208   ⇨ sync the clocks
    MDE_ASAMPTIME = 0xAC | 0x01  # 0b10101100  172   ⇨ set sample rate
    # sample rate determines the rate at which an analog sample is taken.
    # param: (int) (14bits) which sets the sampling interval [10Hz default]
    MDE_ASTOP = 0xB0 | 0x02  # 0b10110000  176   ⇨ set stop condition
    # set the conditions for which sampling stops and Arduino returns to READY
    # parameter is uint16 (13bits)
    # param: 0 then datablobs will return forever until HALT is received [default]
    # param: high bit is 1: treat rest of word as time in ms if 0: rest of word is a count
    MDE_ATRIG = 0xB4 | 0x02  # 0b10110100  180   ⇨ set trigger condition
    # set the conditions for which sampling actually starts. parameter is uint16
    # param: 0 then begin immediately [default]
    # high 3 bits the trigger type 1: button press, 2: rising above threshhold on port
    #            3: falling below threshhold on port 4: trigger on L2H dig port,
    #            5: H2L on digital port
    # next 3 bits is the port to watch 0: Ch1 5V or D1 1: Ch2 5V or D2,
    #                                  2: Ch1 10V, 3: Ch2 10V
    # lowest 10 bits is threshhold for analog values, ignored for digital triggering

    MDE_DTRIG = 0xB8 | 0x01  # 0b10111000  184  ⇨ set the digital transition
    # high nibble of param: port 1 settings: 1: L2H, 2: H2L, 3: ANY transition
    # low nibble of param: port 2 settings: L2H, 2: H2L, 3: ANY transition
    # digital ports trigger when something happens and send a datablob at the
    # moment something the conditions warrant. Since all data has a signature as
    # to where it came from you can easily sort the results with the client. N.B. with
    # vernier digital gates, for example. It is possible to chain multiple gates together
    # on one port. With this configuration there is no way to distinguish which gate
    # triggered the event.

    ST_PORTS = 0xC8 | 0x01   # 0b11001000  200   ⇨ Status of AnalogPorts
    #  STASTOP = 0xC4        # 0b11000100  196   ⇨ stop status
    ST_VER = 0xC8            # 0b11001000  200   ⇨ version info
    # STASTATE = 0xCC        # 0b11001100  204   ⇨ state
    # NOP = 0x88             # 0b10001000  136   ⇨ not used currently

class Trigger:
    IMMEDIATE = 0x00
    FALL_BELOW = 0x01
    RISE_ABOVE = 0x02
    LOW_2_HIGH = 0x01  # LOW is a blocked gate
    HIGH_2_LOW = 0x02  # HIGH is an open gate
    ANY      = 0x03  # EITHER transition triggers

class SampleRates:
    BUTTONPRESS = 0
    S_30_S = 15
    S_10_S = 14
    S_5_S = 13
    S_2_S = 12
    S_1_HZ = 11
    S_5_HZ = 10
    S_10_HZ = 9
    S_20_HZ = 8
    S_40_HZ = 7
    S_50_HZ = 6
    S_100_HZ = 5
    S_200_HZ = 4
    S_500_HZ = 3
    S_1K_HZ = 2
    FASTEST = 1  # approx 1.5 ms per sample

class Sources:
    STR = 0x0
    DIG1 = 0x1
    DIG2 = 0x2
    ANA105 = 0x3
    ANA205 = 0x4
    ANA110 = 0x5
    ANA210 = 0x6
    BTN = 0x7

class VernierShield:
    """
    This is the object that handles all the communication. We want to minimize the number of blocking routines so
    we have a minimum timeout and we need to call a dispatcher to impliment a command
    """
    VERSION = "0.9.7"
    BOOTMSG = "*HELLO*"
    
    @staticmethod
    def default_stringhandler(thestring):
        print(thestring)

    # default blob handler just prints out the data in the following format
    #         src (1 or 2), seq (seq value indicating order), data (for dig an open or close), deltime (time in sec)
    @staticmethod
    def default_blobhandler(seq, data, deltime, src):
        print(f"{src:2d}, {seq:5d}, {data:10}, {deltime:10f}") # f strings

    # decode a recieved datablob to the appropriate handler
    #         +------+------+------+------+------+------+------+
    #    byte |  0   |  1   |  2   |  3   |   4  |   5  |  6   |
    #         +------+------+------+------+------+------+------+
    #         | seq  # | data  |src|  microseconds since SYNC  |
    #         +------+------+------+------+------+------+------+
    #         | seq  # | data  |src|  microseconds since SYNC  |
    #    bits |   11   | 10    | 3 |           32              |
    #         +------+------+------+------+------+------+------+
    @staticmethod
    def decode_datablob(raw_datablob):
        # pack the data into a defined structure
        blob = bitstring.pack('<7B', raw_datablob[0], raw_datablob[1], raw_datablob[2],
                              raw_datablob[3], raw_datablob[4], raw_datablob[5], raw_datablob[6])
        seq = blob[:11]._getuint()
        data = blob[11:21]._getuint()
        src = blob[21:24]._getuint()
        deltime = blob[24:]._getuint()
        return seq, data, deltime / 1.0E6, src

    # build the object, establish a connection to the arduino/vernier shield
    def __init__(self):
      self.logger = logging.getLogger(__name__)
      self.logger.info(f"Version {self.VERSION},{self.BOOTMSG}")

      self.dig01_handler = self.default_blobhandler
      self.dig02_handler = self.default_blobhandler
      self.ana01_handler = self.default_blobhandler
      self.ana02_handler = self.default_blobhandler
      self.string_handler = self.default_stringhandler

    # context methods for the with construction
    def __enter__(self):
      # future note, if we use context manager library we can expand this a bit:
      #       https://stackoverflow.com/questions/5109507/pass-argument-to-enter
      self.open(portname="/dev/ttyACM0", wait=2)
      return self

    def __exit__(self, type, value, traceback):
      self.close()
      # We let normal endings and KeyboardInterrupts exit normally
      if isinstance(value, KeyboardInterrupt) or value == None:
          return True
      return False
        
    # set the dataHandler for a given dataSource
    def set_data_dataHandler(self, src, blobHandler=None):
        if blobHandler is None:
            blobHandler = self.default_blobhandler

        if src == Sources.DIG1:
            self.dig01_handler = blobHandler
        elif src == Sources.DIG2:
            self.dig02_handler = blobHandler
        elif (src == Sources.ANA105) or (src == Sources.ANA110):
            self.ana01_handler = blobHandler
        elif (src == Sources.ANA205) or (src == Sources.ANA210):
            self.ana02_handler = blobHandler

    # capture the handshake character right after we send the command. This method BLOCKS because it has to.
    # we have set up a condition such that the system needs a single response to a command. N.B. A positive
    # acknowledgement doesn't mean the command was carried out, it merely means the predicate and its parameters
    # are valid.
    def _acknowledge(self):
        cc = b''
        for tries in range(5):
            cc = self.serPort.read(1)
            if cc == b'!':
                return True
            if cc == b'?':
                return False
            time.sleep(0.25)  # wait 1/4 a second

        if cc == b'':
            raise Exception("Acknowledge timeout", cc)
        else:
            raise Exception("Don't understand ack/nak response.", cc)

    # if we restarted our machine then this is the string that signals the Arduino is ready
    def open(self, portname="/dev/ttyACM0", wait=2):
        self.serPort = serial.Serial(portname, baudrate=115200, timeout=wait)
        time.sleep(wait) # the act of opening a port causes the arduino to reset.
        self.logger.info("  open waiting for start", end='')
        for i in range(5):  # Take three shots at this.
            self.logger.info('.', end='')  # visual feedback
            if self.serPort.inWaiting() >= len(self.BOOTMSG):
                handshake = self.serPort.readline()
                if self.BOOTMSG in handshake.decode():
                    self.logger.info(handshake.decode()[:-2] + "-Good")
                    return True
            time.sleep(1.0)  # snooze for a short while
        self.logger.info("Timed out")
        return False

    # send a command, low level. Builds parameters and waits for response.
    def send_command(self, predicate, params=[]):
        self.serPort.write(predicate.to_bytes(1, 'big'))
        if isinstance(params, int):  # this handles the case wehere one parameter in a tuple or list is just the value.
            params = [params]
        for i in range(predicate & 0x3):
            self.serPort.write((params[i] & 0x7F).to_bytes(1, 'big'))
        return self._acknowledge()

    # dispatch a recieved datablob to the appropriate handler
    def dispatch_blob(self, raw_datablob):
        seq, data, deltime, src = self.decode_datablob(raw_datablob)
        # dispatch converted data
        if src == Sources.DIG1:
            if self.dig01_handler:
                self.dig01_handler(seq, data, deltime, src)
        elif src == Sources.DIG2:
            if self.dig02_handler:
                self.dig02_handler(seq, data, deltime, src)
        elif (src == Sources.ANA105):
            if self.ana01_handler:
                self.ana01_handler(seq, data*5.0/1024, deltime, src)
        elif (src == Sources.ANA110):
            if self.ana01_handler:
                self.ana01_handler(seq, (data-512)*10.0/512, deltime, src)
        elif (src == Sources.ANA205):
            if self.ana02_handler:
                self.ana02_handler(seq, data, deltime, src)
        elif  (src == Sources.ANA210):
            if self.ana02_handler:
                self.ana02_handler(seq, (data-512)*10.0/512, deltime, src)

    # halt the data taking
    def halt_data(self):
        return self.send_command(Commands.HALT)

    # arm the trigger
    def arm_data(self, chanlist=0):
        if isinstance(chanlist, int):  # in case someone gave us a single value.
            chanlist = [chanlist]
        chanlist.append(0)
        chanlist.reverse()
        chan = reduce(lambda sm, e: sm + (1 << (e-1)), chanlist)
        return self.send_command(Commands.ARM, chan)

    def sync_clocks(self):
        return self.send_command(Commands.MDE_SYNC)

    # displatch a received string
    def dispatch_string(self, raw_string):
        if self.string_handler:
            self.string_handler(raw_string[:-2].decode("utf-8"))

    # call this routine regularly, It scans the serial port for characters and dispatches the correct handle based
    # based on characters it gets.  This intended to be non-blocking
    def loop(self):
        if self.serPort.inWaiting() > 0:
            # get a single character
            cc = self.serPort.read(1)
            if cc == b'\xAA':  # signal the start of a data blob
                # gather 7 additional bytes
                raw_datablob = self.serPort.read(
                    7)  # if we got the starting 0xAA these should come in the requisite timeout
                self.dispatch_blob(raw_datablob)
            if cc == b' ':  # signal we are getting a string
                raw_string = self.serPort.readline()
                self.dispatch_string(raw_string)

    # this is a blocking routine that seeks to get either a string or a datablob response. This is a tool to be
    # used for immediate commands like data reads or status
    def wait_for_response(self):
        # wait for the first character and allow a timeout if it doesn't happen
        cc = self.serPort.read(1)
        if cc == b'\xAA':  # signal the start of a data blob
            # gather 7 additional bytes
            rawdatablob = self.serPort.read(7)  # if we got the starting 0xAA these should come in the requisite timeout
            return self.decode_datablob(rawdatablob)
        elif cc == b' ':  # signal we are getting a string
            rawstring = self.serPort.readline()
            return rawstring[:-2]

    # convenience tool for blinking the
    def blink_led(self, times=1, period=0):
        param = ((times & 0x7) << 4) + (period & 0xF)
        return self.send_command(Commands.BLINKLED, param)

    # set the sampling rate for the analog channels
    def set_sample_rate(self, index=SampleRates.S_10_HZ):
        return self.send_command(Commands.MDE_ASAMPTIME, index)

    # set the trigger conditions for analog channel
    def set_trigger_condition(self, condition=Trigger.IMMEDIATE, level=512):
        channel = 3  # both channels
        # port = 3
        # if port <= 3:
        #     channel = port  # the provided value is already in terms of channel
        # if (port == Sources.ANA105) or (port == Sources.ANA110):
        #     channel = 1  # provided value is in terms of source
        # if (port == Sources.ANA205) or (port == Sources.ANA210):
        #     channel = 2
        param2 = level & 0x7F
        param1 = (level >> 7) + (condition << 5) + (channel << 3)
        return self.send_command(Commands.MDE_ATRIG, [param1, param2])

    # set the stop condition for the analog channel.
    def set_stop_condition(self, num_points):
        num_points = int(num_points)  # in case someone passed a float
        return self.send_command(Commands.MDE_ASTOP, [0x7F & (num_points >> 7), 0x7F & num_points])

    # get details of analog status
    def get_status(self, chanlist=None):
        # Note about an interesting bug: if I use chanlist as a local variable and append to it
        #                                the default values get changed. Who knew? A serious python
        #                                programmer. One solution is to make a 'deep copy' but this seems
        #                                a better way:
        if chanlist == None:
            chanlist = [Sources.ANA105, Sources.ANA205, Sources.DIG1, Sources.DIG2]
        print(chanlist)  # DEBUG
        if isinstance(chanlist, int): # in case someone gave us a single value.
            chanlist = [chanlist]
        numChannels = len(chanlist)
        chanlist.append(0)
        chanlist.reverse()
        chan = reduce(lambda sm, e: sm | (1 << e), chanlist)
        chan = chan >> 1
        
        if self.send_command(Commands.ST_PORTS, chan):
            ans = []
            for i in range(numChannels):
                bstr = self.wait_for_response()
                if bstr != None:
                    ans.append(bstr.decode('UTF-8'))
                else:
                    return ans # failure, return raw value
            try:
                return json.loads("{" + (",".join(ans)) + "}")
            finally:
                return ans
        return "Communication Failed."

    # set the conditions for the digital trigger
    def set_digital_trigger(self, trigger_conditions=[Trigger.ANY]):
        # print("set_digital_trigger")
        if isinstance(trigger_conditions,int):
            trigger_conditions = [trigger_conditions,trigger_conditions]
        if len(trigger_conditions) == 1:
            trigger_conditions.append(trigger_conditions[0])
        trig = (trigger_conditions[1]<<4) + trigger_conditions[0]
        # print(trigger_conditions)
        # print(f"trig: 0x{trig:2X}")
        self.send_command(Commands.MDE_DTRIG, trig)

    # get version of the shield firmware
    def get_version(self):
        if self.send_command(Commands.ST_VER):
            bstr = self.wait_for_response()
            return bstr.decode('UTF-8')
        return "Communication Failed."

    # close the serial port
    def close(self):
        self.serPort.close()

    # timed loop, run loop for a set time.  Will block for specified time
    def run_loop(self, timelimit=10.0):
        now = time.monotonic()
        while (time.monotonic()-now) < timelimit:
            self.loop()

