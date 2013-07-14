
#include "FastSPI_LED2.h"

#define CONTROL_PIN 11

#define NUM_LEDS 240
#define USE_LEDS 120

struct CRGB leds[NUM_LEDS];

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
  
  Serial.begin(115200); 
  
  //Serial.print("Ready with ");
  //Serial.print(USE_LEDS);
  //Serial.println(" LEDs");
}

void loop() { 
  // signal ready to receive data
  Serial.println("x");
  
  if (Serial.readBytes((char*) leds, USE_LEDS * 3) == USE_LEDS * 3) {
    //Serial.println("read full line");
    LEDS.show();
  } else {
    //Serial.println("did not get enough data"); 
  }
}

// z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  

