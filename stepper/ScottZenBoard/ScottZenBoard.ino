/*
  Program for Scott Moorhead's "zen" board

*/

#include <AccelStepper.h>
#define HALFSTEP 8

// Motor pin definitions
#define motorPin1  10     // IN1 on the ULN2003 driver 1
#define motorPin2  11     // IN2 on the ULN2003 driver 1
#define motorPin3  12     // IN3 on the ULN2003 driver 1
#define motorPin4  13     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

int stepsPer90Degrees = 1019;

void setup() {
  delay(100);
  
  // TODO home the steppers 
  
  stepper1.setMaxSpeed(1200.0);
  stepper1.setAcceleration(800.0);
  stepper1.setSpeed(400);
}

void loop() {

  //Change direction when the stepper reaches the target position
  if (stepper1.distanceToGo() == 0) {
    delay(500);
    stepper1.moveTo(stepper1.currentPosition() + stepsPer90Degrees);
  }
  stepper1.run();
}

