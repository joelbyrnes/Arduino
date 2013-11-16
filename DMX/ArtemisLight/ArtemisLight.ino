  // - - - - -
// DmxSerialRecv.pde: Sample DMX application for retrieving 3 DMX values:
//
// address 1 (green)  -> PWM Port 9
// address 2 (blue)   -> PWM Port 10
// address 3 (red)    -> PWM Port 11
// 
// Copyright (c) 2011 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// Modified for use with  Artemis Spaceship Bridge Simulator
// 
// Documentation and samples are available at http://www.mathertel.de/Arduino
// 25.07.2011 creation of the DmxSerial library.
// 10.09.2011 fully control the serial hardware register
//            without using the Arduino Serial (HardwareSerial) class to avoid ISR implementation conflicts.
// 01.12.2011 include file and extension changed to work with the Arduino 1.0 environment
// 28.12.2011 changed to channels 1..3 (RGB) for compatibility with the DmxSerialSend sample.
// 10.05.2012 added some lines to loop to show how to fall back to a default color when no data was received since some time.
// 30.04.2013  removed Yellow to act like a simple 3-channel DMX light and moved writes to bottom of loop() <ethan.dicks@gmail.com>
//
// - - - - -

//#include <DMXSerial.h>
#include "FastSPI_LED2.h"

#define CONTROL_PIN 11

#define NUM_LEDS 180
#define USE_LEDS 180

struct CRGB leds[NUM_LEDS];

#define GreenDefaultLevel 0
#define BlueDefaultLevel  0
#define RedDefaultLevel  80
 
int GreenValue  = GreenDefaultLevel;
int BlueValue   = BlueDefaultLevel;
int RedValue    = RedDefaultLevel;

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  
  LEDS.setBrightness(255);

  LEDS.addLeds<WS2811, CONTROL_PIN, GRB>(leds, NUM_LEDS);
   
  LEDS.clear();
  
  // load with nice blue
  for (int i = 0; i < USE_LEDS; i++) {
    leds[i] = CRGB(0, 8, 16);  
  }
  LEDS.show();
  
  DMXSerial.init(DMXReceiver);
 
  // set some default values
  DMXSerial.write(1, GreenDefaultLevel);
  DMXSerial.write(2, BlueDefaultLevel);
  DMXSerial.write(3, RedDefaultLevel);
}
 
void loop()
{
  // Calculate how long no data packet was received
  unsigned long lastPacket = DMXSerial.noDataSince();
   
  if (lastPacket < 5000) {
    // read recent DMX values and set pwm levels 
    GreenValue  = DMXSerial.read(1);
    BlueValue   = DMXSerial.read(2);
    RedValue    = DMXSerial.read(3);
  }
  else {
    // Show pure red color, when no data was received since 5 seconds or more.
    GreenValue  = GreenDefaultLevel;
    BlueValue   = BlueDefaultLevel;
    RedValue    = RedDefaultLevel;
  }
 
//  analogWrite(GreenPin,  GreenValue);
//  analogWrite(BluePin,   BlueValue);
//  analogWrite(RedPin,    RedValue);
   
}
