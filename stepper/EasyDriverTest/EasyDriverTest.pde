////////////////////////////////////////////////////////
// Stepper Motor skecth for use with the EasyDriver 3.1 
////////////////////////////////////////////////////////

// Dan Thompson 2008
//
// Inpired by the code and chat on this thread.
// http://forum.sparkfun.com/viewtopic.php?t=10378&highlight=easydriver
//
// Use this code at your own risk.
// For all the product details visit http://greta.dhs.org/EasyDriver/
// For the full tutorial visit http://danthompsonsblog.blogspot.com/
// 
 
int dirpin = 2;
int steppin = 5;
int steps = 3200; // 3200 full rotation with microstepping
int delayus = 100; // minimum 100 with microstepping
int delayChangeDirection = 100; // minimum 12 with microstepping

void setup() {
Serial.begin(57600);

pinMode(dirpin, OUTPUT);
pinMode(steppin, OUTPUT);
}
void loop()
{

  int i;

  digitalWrite(dirpin, LOW);     // Set the direction.
  delay(delayChangeDirection);

Serial.println(">>");
  for (i = 0; i<steps; i++)       // Iterate for 4000 microsteps.
  {
digitalWrite(steppin, LOW);  // This LOW to HIGH change is what creates the
digitalWrite(steppin, HIGH); // "Rising Edge" so the easydriver knows to when to step.
    delayMicroseconds(delayus);      // This delay time is close to top speed for this
  }                              // particular motor. Any faster the motor stalls.
  
  reverse();
}

void reverse() {
  int i;
  
  digitalWrite(dirpin, HIGH);    // Change direction.
  delay(delayChangeDirection);

  Serial.println("<<");
  for (i = 0; i<steps; i++)       // Iterate for 4000 microsteps
  {
digitalWrite(steppin, LOW);  // This LOW to HIGH change is what creates the
    digitalWrite(steppin, HIGH); // "Rising Edge" so the easydriver knows to when to step.
    delayMicroseconds(delayus);      // This delay time is close to top speed for this
  }                              // particular motor. Any faster the motor stalls.

}


