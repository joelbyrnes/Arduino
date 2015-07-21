/*
  Program for Scott Moorhead's "zen" board

A :  3   4   5   6   7   8   3   (repeat)   36
B :  4   5   6   7   8   3   4   (repeat)   37
C :  5   6   7   8   3   4   5   (repeat)   38
D :  6   7   8   3   4   5   6   (repeat)   39
E :  7   8   3   4   5   6   7   (repeat)   40
F :  8   3   4   5   6   7   8   (repeat)   41 

move 90 degrees after x minutes. 

p0    p1    p2    p3    p0    p1    p2    p3     p0?

A :  3     4     5     6     7     8     3 
      t=3   t=7   t=12  t=18  t=25  t=33  t=36
B :  4     5     6     7     8     3     4 
      t=4   t=9   t=15  t=22  t=30  t=33  t=37
etc

*/

#include <AccelStepper.h>
#include <JeeLib.h>

// Motor pin definitions
#define motorPin1  10     // IN1 on the ULN2003 driver 1
#define motorPin2  11     // IN2 on the ULN2003 driver 1
#define motorPin3  12     // IN3 on the ULN2003 driver 1
#define motorPin4  13     // IN4 on the ULN2003 driver 1

#define MAX_SPEED  1200.0     // steps per second

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

int stepsPer90Degrees = 1019;

const int sensorPin = A1;    // pin that the sensor is attached to
const int lightThreshold = 100;
const int ledPin = 8;

int stopOnMark;
int found;

// TODO remove?
MilliTimer timer1;

enum {SEQ_A, SEQ_B, SEQ_C, SEQ_D, SEQ_E, SEQ_F, PRINT_TIME, TASK_LIMIT};
// task for homing? 

Scheduler scheduler (TASK_LIMIT);

const int sequenceA[] = {3, 4, 5, 6, 7, 8, 3};
const int sequenceB[] = {4, 5, 6, 7, 8, 3, 4};
const int sequenceC[] = {5, 6, 7, 8, 3, 4, 5};
const int sequenceD[] = {6, 7, 8, 3, 4, 5, 6};
const int sequenceE[] = {7, 8, 3, 4, 5, 6, 7};
const int sequenceF[] = {8, 3, 4, 5, 6, 7, 8};

int seqAPos = 0;
int seqBPos = 0;
int seqCPos = 0;
int seqDPos = 0;
int seqEPos = 0;
int seqFPos = 0;

// control how long the sequence times are, if they were in seconds
int timeFactor = 10;

unsigned int ms;

// TODO remove
int stepper1Rotations;

void setup() {
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);
  
  Serial.begin(57600); 

  delay(100);
  
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper1.setAcceleration(800.0);
  stepper1.setSpeed(400);
  
  // stepper2....
  
  // tenths of a second, ie 5 = 0.5s
  scheduler.timer(SEQ_A, sequenceA[seqAPos++] * timeFactor);
//  scheduler.timer(SEQ_B, sequenceB[seqBPos++] * timeFactor); ...
  scheduler.timer(PRINT_TIME, 10);

  // initially look for the home marker
  stopOnMark = 1;
}

void loop() {
  ms = millis();
  
  int lightValue = analogRead(sensorPin);

  // light up LED if we have found the mark
  if (lightValue > lightThreshold) {
    digitalWrite(ledPin, HIGH);
    found = 1;
  } else {
    digitalWrite(ledPin, LOW);
    found = 0;
  }
  
  doWhatNow();
//  OLD();

  stepper1.run();
  // stepper2.run(); ...
}

void doWhatNow() {
  // TODO home all steppers before starting sequences
  homing();
  
  if (stepper1.distanceToGo() == 0) {
    Serial.println();
    Serial.print("*** stepping done. rotations = ");
    Serial.println(stepper1Rotations);
    Serial.print("found = ");
    Serial.println(found);
    Serial.print("stopOnMark = ");
    Serial.println(stopOnMark);
  }
  
  switch(scheduler.poll()) {
    case SEQ_A: 
      Serial.print("SEQ_A pos = ");
      Serial.println(seqAPos);
      
      // schedule again according to sequence
      scheduler.timer(SEQ_A, sequenceA[seqAPos++] * timeFactor);
      stepper1.setMaxSpeed(MAX_SPEED);
      stepper1.move(stepsPer90Degrees);
      
      // wrap back to start
      if (seqAPos == 7) seqAPos = 0;
      break;

    case PRINT_TIME: 
      scheduler.timer(PRINT_TIME, 10);
      Serial.println(ms);
      break;
  }
  
}

void homing() {
  // seek the mark, repeatedly 
  if (stopOnMark && !found && stepper1.distanceToGo() == 0) {
    // move slowly until we find it, because motor will stop suddenly
    stepper1.setMaxSpeed(MAX_SPEED / 4);
    stepper1.move(stepsPer90Degrees * 4);
  }
  
  if (stopOnMark && found) {
    // return to position where we first found marker
    stepper1.moveTo(stepper1.currentPosition());
    stopOnMark = 0;
  }
}


void OLD() {
  homing();
  
  // debug
  if (stepper1.distanceToGo() == 0) {
    Serial.println();
    Serial.print("*** stepping done. rotations = ");
    Serial.println(stepper1Rotations);
    Serial.print("found = ");
    Serial.println(found);
    Serial.print("stopOnMark = ");
    Serial.println(stopOnMark);
  }
  
  // move 90 degrees at a time
  if (stepper1.distanceToGo() == 0) {

    if (stepper1Rotations == 4) {
      // have rotated 360 degrees
      Serial.println();
      if (!found) {
        Serial.println("### have turned 360 degrees. homing now. ");
        // seek to home before continuing
        stopOnMark = 1;
      } else {
        Serial.println("### have turned 360 degrees and at home. ");
        if (timer1.poll(2000))
          stepper1Rotations = 0;
      }

    } else {
    //  rotate(stepper1);
      stepper1.setMaxSpeed(MAX_SPEED);
      stepper1.move(stepsPer90Degrees);
      stepper1Rotations += 1;
    }      

    
  }

}

