// this version uses MIDI so it requires the hardware serial, so no debugging on arduino pro

#include "FastLED.h"

#define CONTROL_PIN 11

#define NUM_LEDS 170

CRGB leds[NUM_LEDS];

struct Input {
  int inputPin;    // voltage-divider circuit attached to analog pin X
  int pwmPin;      // pin to output PWM on
  int raw;         // analog value read 
  
  int note;        // MIDI note value in hex eg middle C == 0x48
  int velocity;    // track whether note is off
}; 

Input inputA { A0, 3, 0, 0x39, 0 }; // A

#define NUM_INPUTS 1
Input *inputs[NUM_INPUTS] = { &inputA }; 

// TODO use define for pins

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

void loop() {
  // read all analog inputs at once
  for (int i=0; i < NUM_INPUTS; i++) {
    int raw = analogRead(inputs[i]->inputPin);
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
    inputs[i]->raw = h_total / NUM_SAMPLES;
    
    //inputs[i]->raw = raw;
    if (inputs[i]->raw < rawMin) rawMin = inputs[i]->raw;
    if (inputs[i]->raw > rawMax) rawMax = inputs[i]->raw;   // never decrease? maybe over time? 
  }
  
  // process inputs
  for (int i=0; i < NUM_INPUTS; i++) {
    ledValue = map(inputs[i]->raw, rawMin, rawMax, 0, 255);
    //avg = int(float(avg) * 0.99 + float(ledValue) * 0.01);
    corrected = constrain(ledValue, 0, 255); // values are constrained to ensure it is in map, plus will not turn off outside range
    
    analogWrite(inputs[i]->pwmPin, corrected);  
    
    if (millis() > ledTime + ledInterval) {
      ledTime = millis(); 
      
      if (1) { // ledValue >= 10
        // push array along by 1, and set the new value to the 1st 
        memmove8(&leds[1], &leds[0], (NUM_LEDS - 1) * sizeof(struct CRGB));
        leds[0].setRGB(0, corrected, 0);
        FastLED.show();
      }
    }
  }
  
  if (millis() > time + 100) {
    time = millis(); 
    Serial.print("Value: ");    
    Serial.print(inputs[0]->raw);  
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

