// this version uses MIDI so it requires the hardware serial, so no debugging on arduino pro

#include "FastSPI_LED2.h"
#include "cie1931.h"   // https://jared.geek.nz/2013/feb/linear-led-pwm

#define CONTROL_PIN 11

#define NUM_LEDS 1

struct CRGB leds[NUM_LEDS];

struct Input {
  int inputPin;    // voltage-divider circuit attached to analog pin X
  int pwmPin;      // pin to output PWM on
  int raw;         // analog value read 
  
  int note;        // MIDI note value in hex eg middle C == 0x48
  int velocity;    // track whether note is off
}; 

Input inputA { A0, 3, 0, 0x39, 0 }; // A
Input inputB { A1, 5, 0, 0x3B, 0 }; // B
Input inputC { A2, 6, 0, 0x3C, 0 }; // middle C is 0x48

#define NUM_INPUTS 3
Input *inputs[NUM_INPUTS] = { &inputA, &inputB, &inputC }; 

// TODO use define for pins

//int analogPin = 0;     // voltage-divider circuit attached to analog pin 0
//int pwmPin = 3;     
int pulsePin = 12;     
//int raw = 0;           // variable to store the raw input value
unsigned long time = 0;         
int ledValue = 0;
int avg = 0;
int corrected = 0;

int rawMin = 400;
int rawMax = 600;

int pulseValue = 0;
unsigned long pulseTime = 0;
int pulseDir = 1;

//int note = 0x3C; // note/pitch 0x50 chosen randomly
unsigned long midiTime = 0;
//int midiValue = 0;

void setup() {
  Serial.begin(31250);             // Setup serial
  allOff(); // turn off all notes -- does not fix sustain or other settings randomly set by programming
  
  for (int i=0; i < NUM_INPUTS; i++) {
    pinMode(inputs[i]->inputPin, INPUT);    
    pinMode(inputs[i]->pwmPin, OUTPUT);    
  }
  
  // set up pulse/strobe  
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
  
  // read all analog inputs at once
  for (int i=0; i < NUM_INPUTS; i++) {
    inputs[i]->raw = 1024 - analogRead(inputs[i]->inputPin);
    if (inputs[i]->raw < rawMin) rawMin = inputs[i]->raw;
    if (inputs[i]->raw > rawMax) rawMax = inputs[i]->raw;   // never decrease? maybe over time? 
  }
  
  // process inputs
  for (int i=0; i < NUM_INPUTS; i++) {
    ledValue = map(inputs[i]->raw, rawMin, rawMax, 0, 255);
    //avg = int(float(avg) * 0.99 + float(ledValue) * 0.01);
    corrected = cie[constrain(ledValue, 0, 255)]; // values are constrained to ensure it is in map, plus will not turn off outside range
    
    analogWrite(inputs[i]->pwmPin, corrected);  
    
    LEDS.showColor(CRGB(0, corrected, 0));
    
    // max midi command rate is approx 1000/s 
    if (millis() > midiTime + 10) {
      midiTime = millis(); 
  
      if (ledValue >= 30) {
        int velocity = map(ledValue, 0, 255, 0, 127);
  
        // if this is first note, send note on
        if (inputs[i]->velocity == 0 && velocity > 0) {
          midiSend(0x90, inputs[i]->note, velocity);
          // else send aftertouch -- TODO does the controller/instrument support polyphonic (0xA0) or just channel (0xD0)? 
        } else if (velocity != inputs[i]->velocity) {
          midiSend(0xA0, inputs[i]->note, velocity);
        }
        inputs[i]->velocity = velocity;
      } else if (ledValue < 30) {
        // don't bother sending messages when already 0 - TODO should we use note off cmd instead? 
        if (inputs[i]->velocity > 0) {
          inputs[i]->velocity = 0;
          midiSend(0x90, inputs[i]->note, 0x00);
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
}

void allOff() {
  for (int note = 0x00; note < 0x80; note ++) {
    midiSend(0x80, note, 0x00);
    delay(5);
  }
}

// send a MIDI command. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void midiSend(int cmd, int note, int velocity) {
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
  Serial.write(note);
  Serial.write(velocity);
}

// running status - using existing command, send only updates
void runningStatus(int note, int velocity) {
  Serial.write(note);
  Serial.write(velocity);
}

