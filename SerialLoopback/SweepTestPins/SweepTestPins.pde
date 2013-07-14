// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  Serial.begin(57600);
} 


void setupServo(int pin) {
  myservo.detach();
  Serial.print("attaching to pin ");
  Serial.println(pin);
  myservo.attach(pin); 
}
 
void loop() 
{ 
  testServoPin(1);
  testServoPin(2);
  testServoPin(3);
  testServoPin(4);
  testServoPin(5);
  testServoPin(6);
  testServoPin(7);
  testServoPin(8);
  testServoPin(9);
  testServoPin(10);
  testServoPin(11);
  testServoPin(12);
  testServoPin(13);
  testServoPin(14);
  testServoPin(15);
  testServoPin(16);
  testServoPin(17);
  testServoPin(18);
  testServoPin(19);
  testServoPin(20);


}

void testServoPin(int pin) {  
  setupServo(pin);
  
  Serial.println("0 to 180");
  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  Serial.println("180 to 0");
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
} 
