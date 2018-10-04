#include "cie1931.h"   // https://jared.geek.nz/2013/feb/linear-led-pwm

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

void setup() {
  Serial.begin(57600);             // Setup serial
  digitalWrite(13, LOW);         // init off
  pinMode(pwmPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);
  analogWrite(pulsePin, pulseValue);
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

}

