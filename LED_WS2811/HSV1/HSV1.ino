
#include "FastSPI_LED2.h"

#define NUM_LEDS 30

struct CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);
    
   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
    LEDS.setBrightness(64);
    LEDS.addLeds<WS2811, 11>(leds, NUM_LEDS);
    LEDS.clear();
}

void loop() {
    for(int seed = 0; seed < 255; seed++) {
        // do cool stuff with hsv
        CHSV temphsv;
        for(int iLed = 0; iLed < NUM_LEDS; iLed++) {
            // prepare HSV values
            temphsv.h = iLed+seed;
            temphsv.s = 255;
            temphsv.v = 255;
            // convert HSV into RGB, 
            // storing directly into the led array
            hsv2rgb_rainbow( temphsv, leds[iLed]);
        }
        
        LEDS.show();
        delay(5);
    }
}

