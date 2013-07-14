#include <NewSoftSerial.h>

#define POINT_SPEED 9600

NewSoftSerial point1(30, 31);
NewSoftSerial point2(32, 33);
NewSoftSerial point3(34, 35);
NewSoftSerial point4(36, 37);
NewSoftSerial recv(28, 29);

void setup() {
  Serial.begin(9600);
  Serial2.begin(POINT_SPEED);
  point1.begin(POINT_SPEED);
  point2.begin(POINT_SPEED);
  point3.begin(POINT_SPEED);
  point4.begin(POINT_SPEED);
  
  Serial.println("test begins");
}

int count = 0;

void loop() {
  point1.print("pt1-");
  point1.println(count);
  point2.print("pt2-");
  point2.println(count);
  point3.print("pt3-");
  point3.println(count);
  point4.print("pt4-");
  point4.println(count);
  count++;
  
  while(Serial2.available()) {
    Serial.write(Serial2.read());
  }
}


