import sys
import serial
import time
import array
import Image

# NUM_LEDS = 30

line_num = 0
image = None

def get_line():
    global image, line_num
    width, height = image.size
    line = ""

    # how to turn a list of tuples into a list of bytes? generator?

    line_pixels = list(image.getdata())[width * line_num:width * (line_num+1)]
    for pixel in line_pixels:
        line += array.array('B', list(pixel)).tostring()

    # print a monochrome version to console for debugging
    for b in line_pixels:
        print "%s" % hex(sum(b) / len(b)),
    print

    line_num += 1
    if line_num == height:
        print 'reached end of image'
        line_num = 0

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


def main(port_path, image_path):
    global image
    port = None
    try:
        port = serial.Serial(port_path, 115200, timeout=300)
        print port.portstr

        image = Image.open(image_path)
        print image.format, image.size, image.mode

        loop(port)

    except KeyboardInterrupt:
        print ' exiting.'
        port.close()

if __name__ == '__main__':
    if len(sys.argv) > 2:
        # eg /dev/tty.usbserial-A900epwQ test1.png
        main(sys.argv[1], sys.argv[2])
    else:
        print "not enough args: serial_device image_file"


