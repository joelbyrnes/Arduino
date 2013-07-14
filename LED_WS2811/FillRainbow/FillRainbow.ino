#include "FastSPI_LED2.h"

#define NUM_LEDS 240
#define USE_LEDS 240

struct CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);
  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
    LEDS.setBrightness(255);
    LEDS.addLeds<WS2811, 11, GRB>(leds, NUM_LEDS);
    LEDS.clear();
    //black();
}


void black() {
  for(int iLed = 0; iLed < NUM_LEDS; iLed++) {
    LEDS.showColor(CRGB(0, 0, 0));
  }
  LEDS.show();
}


void loop() {
  static uint8_t startinghue = 0;
  startinghue = startinghue - 1;
  fill_rainbow( leds, USE_LEDS, startinghue);
  LEDS.show();
  delay(5);
}
