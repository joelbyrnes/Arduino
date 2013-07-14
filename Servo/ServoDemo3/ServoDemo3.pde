// Sweep
// by BARRAGAN <http://barraganstudio.com> 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  Serial.begin(19200);
  myservo.attach(15);  // attaches the servo on pin 9 to the servo object 
} 
 

void loop() 
{ 
  if (Serial.available() > 0) {
    int in = Serial.read();
    Serial.println(in);
    if (in != -1) {
      myservo.write(in);
    }
  }
  
}
void sweep() {
  
  // goes from 0 degrees to 180 degrees 
  for(pos = 18; pos < 20; pos += 1) {                                  
    // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    Serial.println(pos);
    delay(2000);                       // waits 15ms for the servo to reach the position 
  } 
      // goes from 180 degrees to 0 degrees 
  for(pos = 20; pos>=18; pos-= 1) {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    Serial.println(pos);
    delay(2000);                       // waits 15ms for the servo to reach the position 
  }
  
} 
