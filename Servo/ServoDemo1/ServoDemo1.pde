#include <Servo.h>
#include <NewSoftSerial.h>

Servo servo1;

void setup() {
  pinMode(1,OUTPUT);
  servo1.attach(15);
  
  Serial.begin(19200);
  Serial.println("Hello, world?");
}

void loop() {
  static int v = 0;

  if ( Serial.available()) {
    char ch = Serial.read();
    Serial.print(ch);

    switch(ch) {
      case '0'...'9':
        v = v * 10 + ch - '0';
        break;
      case 's':
        servo1.write(v);
        v = 0;
        break;
    }
  }
} 


