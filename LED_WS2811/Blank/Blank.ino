
#include "FastSPI_LED2.h"

#define NUM_LEDS 240

#define USE_LEDS 30

struct CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(64);

  LEDS.addLeds<WS2811, 11>(leds, NUM_LEDS);
   
  LEDS.clear();
}

void loop() { 
  for(int iLed = 0; iLed < NUM_LEDS; iLed++) {
    leds[iLed] = CRGB(0, 0, 255); 
  }

  LEDS.show();
  delay(100);
  
  LEDS.showColor(CRGB(255, 0, 0));
  delay(100);
  
  LEDS.showColor(CRGB(0, 255, 0));
  //LEDS.show();
  delay(100);
  
}
