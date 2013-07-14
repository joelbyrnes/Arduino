#include <Servo.h>
#include <NewSoftSerial.h>

Servo servo1;
NewSoftSerial serial(4, 5);

void setup() {
  pinMode(1,OUTPUT);
  servo1.attach(15);
  
  serial.begin(19200);
  serial.println("Hello, world?");
}

void loop() {
  static int v = 0;

  if ( serial.available()) {
    char ch = serial.read();

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


