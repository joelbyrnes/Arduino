#include <Servo.h>
#include <NewSoftSerial.h>

Servo tilt;
Servo pan;

void setup() {
  pinMode(1,OUTPUT);
  tilt.attach(14);
  pan.attach(15);
  
  Serial.begin(19200);
  Serial.println("Hello, world?");
}

void loop() {
  // center
  pan.write(90);
  tilt.write(90);
  delay(1000);
  
  sweep(pan, 90, 0, 15);
  sweep(pan, 0, 180, 15);
  sweep(pan, 180, 0, 15);
  sweep(tilt, 90, 0, 15);
  sweep(tilt, 0, 180, 15);
  sweep(tilt, 180, 0, 15);
} 

void sweep(Servo servo, int startPos, int endPos, int wait) {
  for(int pos = startPos; pos < endPos; pos += 1) {                                 
    servo.write(pos);            
    delay(wait);                      
  } 
}

