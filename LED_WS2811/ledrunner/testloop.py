import sys
import serial
import time
import array
import Image

#SERIAL_PORT = "/dev/tty.usbserial-A600dVGp"
SERIAL_PORT = "/dev/tty.usbserial-A900epwQ"
NUM_LEDS = 60

line_num = 0
val = 0

def get_line():
    global val
    line = ""

    for i in range(0, NUM_LEDS):
        pixel = array.array('B', [0x00, 0x00, 0x00])
        if i == val:
            pixel = array.array('B', [0x08, 0x08, 0x08])
        line += pixel.tostring()

    val += 1
    if val == NUM_LEDS:
        val = 0

    return line


def loop(port):
    while 1:
        start = time.time()
        data = port.readline()
        end = time.time()
        print "waiting for sync took %f" % (end - start)

        print data,
        if data[0] == "x":
            line = get_line()

            start = time.time()
            port.write(line)
            end = time.time()
            print "writing line took %f" % (end - start)

            for b in line:
                print "%s" % hex(ord(b)),
            print


def main():
        port = None
        try:
                port = serial.Serial(SERIAL_PORT, 115200, timeout=300)
                print port.portstr
                loop(port)

        except KeyboardInterrupt:
                print ' exiting.'
                port.close()

if __name__ == '__main__':
    main()


