// this version uses MIDI so it requires the hardware serial, so no debugging on arduino pro

#include "FastSPI_LED2.h"
#include "cie1931.h"   // https://jared.geek.nz/2013/feb/linear-led-pwm

#define CONTROL_PIN 11

#define NUM_LEDS 1

struct CRGB leds[NUM_LEDS];

// TODO use define for pins

int ledPin = 13;
int analogPin = 0;     // voltage-divider circuit attached to analog pin 0
int pwmPin = 3;     
int pulsePin = 5;     
int raw = 0;           // variable to store the raw input value
unsigned long time = 0;         
int ledValue = 0;
int avg = 0;
int corrected = 0;

int rawMin = 511;
int rawMax = 512;

int pulseValue = 0;
unsigned long pulseTime = 0;
int pulseDir = 1;

int note = 0x3C; // note/pitch 0x50 chosen randomly
unsigned long midiTime = 0;
int midiValue = 0;

void setup() {
  Serial.begin(31250);             // Setup serial
  //allOff(); // turn off all notes
  
  digitalWrite(ledPin, LOW);         // init off
  pinMode(pwmPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);
  analogWrite(pulsePin, pulseValue);
  
  LEDS.setBrightness(64);

  LEDS.addLeds<WS2811, CONTROL_PIN, GRB>(leds, NUM_LEDS);
   
  LEDS.clear();
  
  // load with nice blue
  LEDS.showColor(CRGB(0, 8, 16));
  
}

void loop() {
  if (millis() > pulseTime + 20) {
    pulseTime = millis(); 
    pulseValue = pulseValue + 5 * pulseDir; 
    
    if (pulseValue > 255) {
      pulseValue = 255;   
      pulseDir = -1;
    } else if (pulseValue < 0) {        
      pulseValue = 0; 
      pulseDir = 1;
      //pulseDir = -1 * pulseDir;
    } 

    analogWrite(pulsePin, cie[pulseValue]);
  }
  
  raw = 1024 - analogRead(analogPin);    // Reads the Input PIN
  if (raw < rawMin) rawMin = raw;
  if (raw > rawMax) rawMax = raw;   // never decrease? maybe over time? 
  
  ledValue = map(raw, rawMin, rawMax, 0, 255);
  //avg = int(float(avg) * 0.99 + float(ledValue) * 0.01);
  corrected = cie[constrain(ledValue, 0, 255)]; // values are constrained to ensure it is in map, plus will not turn off outside range
  
  analogWrite(pwmPin, corrected);  
  
  LEDS.showColor(CRGB(0, corrected, 0));
  
  // max midi command rate is approx 1000/s 
  if (millis() > midiTime + 10) {
    midiTime = millis(); 

    if (ledValue >= 30) {
      midiValue = map(ledValue, 0, 255, 0, 127);
      //Note on channel 1 (0x90), some note value (note), some velocity
      noteOn(0x90, note, midiValue);
    } else if (ledValue < 30) {
      // don't bother sending messages when already 0 - TODO should we use note off cmd instead? 
      if (midiValue > 0) {
        midiValue = 0;
        noteOn(0x90, note, 0x00);
      }
    }
  }
  
  /*
  if (millis() > time + 100) {
    time = millis(); 
    Serial.print("Value: ");    
    Serial.print(raw);  
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
  */

}

void allOff() {
  for (int note = 0x00; note < 0x80; note ++) {
    noteOn(0x90, note, 0x00);
    delay(10);
  }
}

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  /*
  Serial.print("cmd: ");    
  Serial.print(cmd);  
  Serial.print(" - pitch: ");    
  Serial.print(pitch);  
  Serial.print(" - velocity: ");    
  Serial.print(velocity);  
  Serial.println("");   
  */
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

