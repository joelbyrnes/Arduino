// read analog input from Sharp gp2y0a51sk positional sensor and drive addressible RGB LED strip 

#include "FastLED.h"
#include "cie1931.h"   // https://jared.geek.nz/2013/feb/linear-led-pwm
#include <JeeLib.h>    // Scheduler

#define SENSOR_PIN A0
#define CONTROL_PIN 11
#define NUM_LEDS 50
#define LED_BRIGHTNESS 255 // max 255
// saturation (HSL) goes to 100% beyond this LED value (8-bit) 
#define FULL_SAT_THRESHOLD 127

#define READ_INTERVAL 1
#define LED_INTERVAL 30
#define DEBUG_INTERVAL 100  // 0 to disable

CRGB leds[NUM_LEDS];

MilliTimer debugTimer, readTimer, writeTimer;

// starting values for analog input, which range to include values seen over time
// min reading is lowest when an object is at the far extent of the range, otherwise it floats higher. 
// tends to increase brightness over time because min goes lower than ambient reading. maybe 
unsigned int rawMin = 160;
unsigned int rawMax = 400;

unsigned int raw = 0;
unsigned int avg = 0;
uint8_t ledValue = 0;
uint8_t hue = 0;   // hue is automatically rotated to give rainbow effect

unsigned long time = 0;

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
#define NUM_SAMPLES 32

unsigned int h_readings[NUM_SAMPLES];      // the readings from the analog input
unsigned int h_index = 0;                  // the index of the current reading
unsigned int h_total = 0;                  // the running total

void setup() {
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.addLeds<WS2811, CONTROL_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  fill_solid(leds, 5, CRGB(50,0,200)); 
  FastLED.show();

  Serial.begin(57600);      
  Serial.println();   
  Serial.print(NUM_LEDS);   
  Serial.print(" LEDS; ");
  debugLog(millis());
  Serial.println("--- starting ---");   
}

unsigned int getSmoothedInput(unsigned int raw) {
  // subtract the oldest reading:
  h_total = h_total - h_readings[h_index];         
  // read from the sensor:  
  h_readings[h_index] = raw; 
  // add the reading to the total:
  h_total = h_total + h_readings[h_index++];       
  // if we're at the end of the array, restart
  if (h_index >= NUM_SAMPLES) h_index = 0;                           
  return h_total / NUM_SAMPLES;
}

void loop() {
  // read the sensor and smooth the value (sometimes spikes) 
  if (readTimer.poll(READ_INTERVAL)) {
    raw = analogRead(SENSOR_PIN);
    if (raw < rawMin) rawMin = raw;
    if (raw > rawMax) rawMax = raw;   // never decrease? maybe over time? 
  
    avg = getSmoothedInput(raw);
    
    // map auto-correlated to input range and reduce to 8 bits
    ledValue = map(avg, rawMin, rawMax, 0, 255);
    //corrected = cie[constrain(ledValue, 0, 255)]; // values are constrained to ensure it is in map, plus will not turn off outside range
  }
  
  // every once in a while push a new value onto the strip 
  if (writeTimer.poll(LED_INTERVAL)) {
    // push array along by 1, and set the new value to the 1st 
    memmove8(&leds[1], &leds[0], (NUM_LEDS - 1) * sizeof(struct CRGB));
    //leds[0].setRGB(0, corrected, 0);
    
    // make saturation dull up to a threshold then full colour
    uint8_t sat = 0;
    if (ledValue > FULL_SAT_THRESHOLD) sat = 255;
    else sat = map(ledValue, 0, FULL_SAT_THRESHOLD, 0, 255);
    
    hue = hue + 1;
    leds[0].setHSV(hue, sat, ledValue);
    if (hue>255) hue = 0;
    FastLED.show();
  }
  
  if (debugTimer.poll(DEBUG_INTERVAL)) {
    debugLog(millis());
  }
}

void debugLog(unsigned int timestamp) {
  Serial.print("[");    
  Serial.print(timestamp);  
  Serial.print("ms] raw: ");    
  Serial.print(raw);  
  Serial.print(" - avg: ");    
  Serial.print(avg);  
  Serial.print(" - min: ");    
  Serial.print(rawMin);  
  Serial.print(" - max: ");    
  Serial.print(rawMax);  
  Serial.print(" - LED: ");    
  Serial.print(ledValue);  
//  Serial.print(" - corrected: ");    
//  Serial.print(corrected);  
  Serial.println(""); 
}

