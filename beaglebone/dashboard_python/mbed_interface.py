import Adafruit_BBIO.UART as UART
import copy
import Queue
import serial
import thread
import time


class MbedInterface(object):
    def __init__(self):
        UART.setup("UART4")
        self.serial = serial.Serial(port = "/dev/ttyO4", baudrate=115200)
        self._read_queue = Queue.Queue()

        thread.start_new_thread(self._read_thread, (self,))

    def get_line(self):
        if not self._read_queue.empty():
            return self._read_queue.get()
        else:
            return None

    def _read_thread(self, args):
        cur_line = ''
        while True:
            char = self.serial.read()
            if char == '\n':
                self._read_queue.put(copy.copy(cur_line))
                cur_line = ''
            else:
                cur_line += char
            time.sleep(0.1)
