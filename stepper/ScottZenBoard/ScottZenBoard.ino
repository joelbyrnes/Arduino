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

TO DO:
* pause switch so boards can be fixed up

*/

#include <AccelStepper.h>
#include <JeeLib.h>

// Motor pin definitions
#define motorPinA1  22     // IN1 on the ULN2003 driver 1
#define motorPinA2  24     // IN2 on the ULN2003 driver 1
#define motorPinA3  26     // IN3 on the ULN2003 driver 1
#define motorPinA4  28     // IN4 on the ULN2003 driver 1

#define motorPinB1  30    
#define motorPinB2  32     
#define motorPinB3  34    
#define motorPinB4  36   

#define motorPinC1  38    
#define motorPinC2  40     
#define motorPinC3  42    
#define motorPinC4  44   

#define motorPinD1  46    
#define motorPinD2  48     
#define motorPinD3  50    
#define motorPinD4  52   

#define motorPinE1  53    
#define motorPinE2  51     
#define motorPinE3  49    
#define motorPinE4  47   

#define motorPinF1  45    
#define motorPinF2  43     
#define motorPinF3  41    
#define motorPinF4  39   

const int sensorPin = A1;    // pin that the sensor is attached to
const int ledPin = 8;

#define MAX_SPEED  1200.0     // steps per second

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
// HALF4WIRE or FULL4WIRE
AccelStepper stepperA(AccelStepper::HALF4WIRE, motorPinA1, motorPinA3, motorPinA2, motorPinA4);
AccelStepper stepperB(AccelStepper::HALF4WIRE, motorPinB1, motorPinB3, motorPinB2, motorPinB4);
AccelStepper stepperF(AccelStepper::HALF4WIRE, motorPinF1, motorPinF3, motorPinF2, motorPinF4);

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

int stepsPer90Degrees = 1019;

int stopAOnMark;
int stopFOnMark;
int foundA;
int foundF;

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

// control how long the sequence times are, if they were in seconds. Note that scheduler works in 10ths of a second. 
int timeFactor = 10;

unsigned int ms;

// TODO remove
int stepperARotations;
int stepperFRotations;

void setup() {
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);
  
  Serial.begin(57600); 

  delay(100);
  
  stepperA.setMaxSpeed(MAX_SPEED);
  stepperA.setAcceleration(800.0);
  stepperA.setSpeed(400);

  stepperB.setMaxSpeed(MAX_SPEED);
  stepperB.setAcceleration(800.0);
  stepperB.setSpeed(400);

  stepperF.setMaxSpeed(MAX_SPEED);
  stepperF.setAcceleration(800.0);
  stepperF.setSpeed(400);

  // tenths of a second, ie 5 = 0.5s
  scheduler.timer(SEQ_A, sequenceA[seqAPos++] * timeFactor);
  scheduler.timer(SEQ_B, sequenceB[seqBPos++] * timeFactor);
  scheduler.timer(SEQ_F, sequenceF[seqFPos++] * timeFactor);
  
  scheduler.timer(PRINT_TIME, 10);

  // initially look for the home marker
  //stopAOnMark = 1;
  //stopFOnMark = 1;
}

void loop() {
  ms = millis();
  
  int lightValue = analogRead(sensorPin);

  // light up LED if we have found the mark
  if (lightValue > lightThreshold) {
    digitalWrite(ledPin, HIGH);
    foundA = 1;
  } else {
    digitalWrite(ledPin, LOW);
    foundA = 0;
  }
  
  doWhatNow();
//  OLD();

  stepperA.run();
  stepperB.run();
  stepperF.run();
}

void doWhatNow() {
  // TODO home all steppers before starting sequences
  //homing();
  
  if (stepperA.distanceToGo() == 0) {
    stepperA.disableOutputs();
  }

  if (stepperB.distanceToGo() == 0) {
    stepperB.disableOutputs();
  }

  if (stepperF.distanceToGo() == 0) {
    stepperF.disableOutputs();
  }

/*
  Serial.print("* scheduler poll = ");
  Serial.println(scheduler.poll(), HEX);
  
  if (stepperA.distanceToGo() == 0) {
    Serial.println();
    Serial.print("*** stepper A done. rotations = ");
    Serial.println(stepperARotations);
    Serial.print("foundA = ");
    Serial.println(foundA);
//    Serial.print("stopAOnMark = ");
//    Serial.println(stopAOnMark);
  }

  if (stepperF.distanceToGo() == 0) {
    Serial.println();
    Serial.print("*** stepper F done. rotations = ");
    Serial.println(stepperFRotations);
    Serial.print("foundF = ");
    Serial.println(foundF);
//    Serial.print("stopFOnMark = ");
//    Serial.println(stopFOnMark);
  }
  */
  
  switch(scheduler.poll()) {
    case SEQ_A: 
      Serial.print("SEQ_A pos = ");
      Serial.println(seqAPos);
      
      // schedule again according to sequence
      scheduler.timer(SEQ_A, sequenceA[seqAPos++] * timeFactor);
      stepperA.enableOutputs();
      stepperA.setMaxSpeed(MAX_SPEED);
      stepperA.move(stepsPer90Degrees);
      
      // wrap back to start
      if (seqAPos == 7) seqAPos = 0;
      break;

    case SEQ_B: 
      Serial.print("SEQ_B pos = ");
      Serial.println(seqBPos);
      
      // schedule again according to sequence
      scheduler.timer(SEQ_B, sequenceB[seqBPos++] * timeFactor);
      stepperB.enableOutputs();
      stepperB.setMaxSpeed(MAX_SPEED);
      stepperB.move(stepsPer90Degrees);
      
      // wrap back to start
      if (seqBPos == 7) seqBPos = 0;
      break;

    case SEQ_F: 
      Serial.print("SEQ_F pos = ");
      Serial.println(seqFPos);
      
      // schedule again according to sequence
      scheduler.timer(SEQ_F, sequenceF[seqFPos++] * timeFactor);
      stepperF.enableOutputs();
      stepperF.setMaxSpeed(MAX_SPEED);
      stepperF.move(stepsPer90Degrees);
      
      // wrap back to start
      if (seqFPos == 7) seqFPos = 0;
      break;

    case PRINT_TIME: 
      scheduler.timer(PRINT_TIME, 10);
      Serial.println(ms);
      break;
  }
  
}

void homing() {
  // seek the mark, repeatedly 
  if (stopAOnMark && !foundA && stepperA.distanceToGo() == 0) {
    // move slowly until we find it, because motor will stop suddenly
    stepperA.setMaxSpeed(MAX_SPEED / 4);
    stepperA.move(stepsPer90Degrees);
  }
  
  if (stopAOnMark && foundA) {
    // return to position where we first found marker
    stepperA.moveTo(stepperA.currentPosition());
    stopAOnMark = 0;
  }
}


void OLD() {
  homing();
  
  // debug
  if (stepperA.distanceToGo() == 0) {
    Serial.println();
    Serial.print("*** stepping done. rotations = ");
    Serial.println(stepperARotations);
    Serial.print("foundA = ");
    Serial.println(foundA);
    Serial.print("stopAOnMark = ");
    Serial.println(stopAOnMark);
  }
  
  // move 90 degrees at a time
  if (stepperA.distanceToGo() == 0) {

    if (stepperARotations == 1) {
      // have rotated 90 degrees
      Serial.println();
      if (!foundA) {
        Serial.println("### have turned 90 degrees. homing now. ");
        // seek to home before continuing
        stopAOnMark = 1;
      } else {
        Serial.println("### have turned 90 degrees and at home. ");
        if (timer1.poll(2000))
          stepperARotations = 0;
      }

    } else {
    //  rotate(stepperA);
      stepperA.setMaxSpeed(MAX_SPEED);
      stepperA.move(stepsPer90Degrees);
      stepperARotations += 1;
    }      
    
  }

}

