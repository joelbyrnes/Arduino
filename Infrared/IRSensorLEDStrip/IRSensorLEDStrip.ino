// this version uses MIDI so it requires the hardware serial, so no debugging on arduino pro

#include "FastLED.h"

#define SENSOR_PIN A0
#define CONTROL_PIN 11
#define NUM_LEDS 170

CRGB leds[NUM_LEDS];

// TODO use more defines

unsigned long time = 0;         
int ledValue = 0;
int avg = 0;
int corrected = 0;

int rawMin = 400;
int rawMax = 600;

unsigned long ledTime = 0;
unsigned int ledInterval = 30;

uint8_t hue = 0;

void setup() {
  Serial.begin(57600);      
//  FastLED.setBrightness(128);
  FastLED.addLeds<WS2811, CONTROL_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  fill_solid(leds, 5, CRGB(50,0,200)); 
  FastLED.show();
  
}

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
#define NUM_SAMPLES 16

int h_readings[NUM_SAMPLES];      // the readings from the analog input
int h_index = 0;                  // the index of the current reading
int h_total = 0;                  // the running total
unsigned int smoothed = 0;

void loop() {
  int raw = analogRead(SENSOR_PIN);
  // smooth input
  
  // subtract the last reading:
  h_total = h_total - h_readings[h_index];         
  // read from the sensor:  
  h_readings[h_index] = raw; 
  // add the reading to the total:
  h_total = h_total + h_readings[h_index++];       

  // if we're at the end of the array, restart
  if (h_index >= NUM_SAMPLES) h_index = 0;                           

  // calculate the average:
  smoothed = h_total / NUM_SAMPLES;
  
  ledValue = map(smoothed, rawMin, rawMax, 0, 255);
  //avg = int(float(avg) * 0.99 + float(ledValue) * 0.01);
  corrected = constrain(ledValue, 0, 255); // values are constrained to ensure it is in map, plus will not turn off outside range
  
  if (millis() > ledTime + ledInterval) {
    ledTime = millis(); 

    // push array along by 1, and set the new value to the 1st 
    memmove8(&leds[1], &leds[0], (NUM_LEDS - 1) * sizeof(struct CRGB));
    leds[0].setRGB(0, corrected, 0);
    FastLED.show();
  }
  
  if (millis() > time + 100) {
    time = millis(); 
    Serial.print("Value: ");    
    Serial.print(smoothed);  
    Serial.print(" - min: ");    
    Serial.print(rawMin);  
    Serial.print(" - max: ");    
    Serial.print(rawMax);  
    Serial.print(" - LED: ");    
    Serial.print(ledValue);  
    Serial.print(" - corrected: ");    
    Serial.print(corrected);  
    Serial.println("");  
  }
}

