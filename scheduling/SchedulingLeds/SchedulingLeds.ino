#include <JeeLib.h>

const int led1Pin = 2;
const int led2Pin = 3;
const int led3Pin = 4;

unsigned long timePassed = 0;

MilliTimer timer1;

int sequence[7] = {3, 4, 5, 6, 7, 8, 3};
int seqPos = 0;

void setup() {
  pinMode(led1Pin, OUTPUT);    
  pinMode(led2Pin, OUTPUT);    
  pinMode(led3Pin, OUTPUT);    
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  
  Serial.begin(57600); 
}

void loop() {
  unsigned long ms = millis() - timePassed;
  unsigned long secs = millis() / 1000;
  
  if (timer1.poll(500)) {
    Serial.println(ms);
  }
 
  if (ms > 1000 && ms < 1500) {
    digitalWrite(led1Pin, HIGH);
  } 
  if (ms > 3000 && ms < 3500) {
    digitalWrite(led1Pin, LOW);
  }

  if (ms > 2000 && ms < 2500) {
    digitalWrite(led2Pin, HIGH);
  } 
  if (ms > 3000 && ms < 3500) {
    digitalWrite(led2Pin, LOW);
  }
  
  if (ms > 3000 && ms < 3500) {
    digitalWrite(led3Pin, HIGH);
  } 
  if (ms > 4000 && ms < 4500) {
    digitalWrite(led3Pin, LOW);
  }

  if (ms > 4000 && ms < 4500) {
    // start loop again
    timePassed = millis();
  }

  
}

