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

    print val
    for i in range(0, NUM_LEDS):
        pixel = array.array('B', [0x0, 0x0, 0x0])
        if i == val:
            pixel = array.array('B', [0x9, 0x9, 0x9])
        line += pixel.tostring()

    val += 1
    if val == NUM_LEDS:
        print "resetting"
        val = 0

    return line


def loop(port):
    while 1:
        line = get_line()

        port.write(line)
        print 'sending end char'
        port.write(array.array('B', [255]).tostring())
        # port.write('!')

        for b in line:
            print "%s" % hex(ord(b)),
        print

        time.sleep(0.023)


def main():
    port = None
    try:
        port = serial.Serial(SERIAL_PORT, 115200, timeout=300)
        print port.portstr
        time.sleep(2)
        loop(port)

    except KeyboardInterrupt:
        print ' exiting.'
        port.close()

if __name__ == '__main__':
    main()


