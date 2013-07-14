
#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 

int inPin = 22; 

void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(20);
  pinMode(inPin, INPUT);
} 
 
 
void loop() 
{ 
  int val = digitalRead(inPin);
  if (val == HIGH) {
    myservo.write(164);
    delay(1000);
  } else {
    myservo.write(20);
  }
} 
