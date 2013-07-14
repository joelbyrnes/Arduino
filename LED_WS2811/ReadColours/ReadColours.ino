
#include "FastSPI_LED2.h"

#define CONTROL_PIN 13

#define NUM_LEDS 240
#define USE_LEDS 30

struct CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(64);

  LEDS.addLeds<WS2811, CONTROL_PIN, GRB>(leds, NUM_LEDS);
   
  LEDS.clear();
  
  for (int i = 0; i < USE_LEDS; i++) {
    leds[i] = CRGB(0, 30, 64);  
  }
  LEDS.show();
  
  Serial.begin(57600); 
  
  // signal read to receive data
  Serial.println("x");
}

int idx = 0;
uint8_t bytes[USE_LEDS * 3];

void loop() { 
  
  while (Serial.available() > 2) {
    // read series of 3 bytes into r,g,b for each led up to USE_LEDS, then show it. 
    // TODO more efficient way to read data directly into data structure?

    //for (int i = 0; i < USE_LEDS; i++) {
      leds[idx].r = Serial.read();
      leds[idx].g = Serial.read();
      leds[idx].b = Serial.read();
    //}
    
    idx++;
    
    //Serial.println("read one led");
    //Serial.println(idx);
  }
  
  if (idx == USE_LEDS) { 
    //Serial.println("row read");
    
    LEDS.show();

    idx = 0;
    
    // signal read to receive data
    Serial.println("x");
  }
}

// z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  z  

