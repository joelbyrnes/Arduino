import sys
import serial
import time
from time import sleep
import array

def set_freq(port, freq):
	print "setting freq %f" % freq
	port.write("freq %f\n" % freq)
	data = port.readline()
	print data,
	data = port.readline()
	print data,

def range_test(port):
	for i in range(0, 1001):
		set_freq(port, i * 10)
		sleep(0.1)
		
def note_pause(port, freq, time, offtime):
	set_freq(port, freq) 
	sleep(time)
	set_freq(port, 0.0) # off
	sleep(offtime)
	
def play_major_scale(port):
	time = 1
	offtime = 0.2
	
	note_pause(port, 261.63, time, offtime) # C4
	note_pause(port, 293.66, time, offtime) # D4
	note_pause(port, 329.63, time, offtime) # E4
	note_pause(port, 349.23, time, offtime) # F4
	note_pause(port, 392.00, time, offtime) # G4
	note_pause(port, 440.00, time, offtime) # A4
	note_pause(port, 493.88, time, offtime) # B4
	note_pause(port, 523.25, time, offtime) # C5
			
def play_scale(port):
	time = 1
	offtime = 0.5
	
	note_pause(port, 261.63, time, offtime) # C4
	note_pause(port, 277.18, time, offtime) # C4#
	note_pause(port, 293.66, time, offtime) # D4
	note_pause(port, 311.13, time, offtime) # D4#
	note_pause(port, 329.63, time, offtime) # E4
	note_pause(port, 349.23, time, offtime) # F4
	note_pause(port, 369.99, time, offtime) # F4#
	note_pause(port, 392.00, time, offtime) # G4
	note_pause(port, 415.30, time, offtime) # G4#
	note_pause(port, 440.00, time, offtime) # A4
	note_pause(port, 466.16, time, offtime) # A4#
	note_pause(port, 493.88, time, offtime) # B4
	note_pause(port, 523.25, time, offtime) # C5
	
def play_C(port):
	set_freq(port, 261.63) # C4
	sleep(10)
	set_freq(port, 0.0) # off

def main(port_path):
	port = None
	try:
		port = serial.Serial(port_path, 57600, timeout=0)
		print port.portstr
		data = None
		while (data == None or len(data) == 0):
			data = port.readline()
			if (data):
				print data,
		data = port.readline()
		print data,
				
		play_major_scale(port)
		#play_C(port)
		#range_test(port)
		
	except KeyboardInterrupt:
		print ' exiting.'
		port.close()

if __name__ == '__main__':
    if len(sys.argv) > 1:
        # eg /dev/tty.usbserial-A900epwQ
        main(sys.argv[1])
    else:
        print "not enough args: serial_device"


