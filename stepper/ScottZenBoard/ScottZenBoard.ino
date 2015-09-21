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
AccelStepper stepperC(AccelStepper::HALF4WIRE, motorPinC1, motorPinC3, motorPinC2, motorPinC4);
AccelStepper stepperD(AccelStepper::HALF4WIRE, motorPinD1, motorPinD3, motorPinD2, motorPinD4);
AccelStepper stepperE(AccelStepper::HALF4WIRE, motorPinE1, motorPinE3, motorPinE2, motorPinE4);
AccelStepper stepperF(AccelStepper::HALF4WIRE, motorPinF1, motorPinF3, motorPinF2, motorPinF4);

#define NUM_STEPPERS 6
AccelStepper *steppers[NUM_STEPPERS] = {&stepperA, &stepperB, &stepperC, &stepperD, &stepperE, &stepperF};

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

int stepsPer90Degrees = 1019;

const int lightThreshold = 100;

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

void setup() {
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);
  
  Serial.begin(57600); 

  delay(100);
  
  for (int i=0; i < NUM_STEPPERS; i++) {
    steppers[i]->setMaxSpeed(MAX_SPEED);
    steppers[i]->setAcceleration(800.0);
    steppers[i]->setSpeed(400);  
  }

  // tenths of a second, ie 5 = 0.5s
  scheduler.timer(SEQ_A, sequenceA[seqAPos++] * timeFactor);
  scheduler.timer(SEQ_B, sequenceB[seqBPos++] * timeFactor);
  scheduler.timer(SEQ_C, sequenceB[seqCPos++] * timeFactor);
  scheduler.timer(SEQ_D, sequenceB[seqDPos++] * timeFactor);
  scheduler.timer(SEQ_E, sequenceB[seqEPos++] * timeFactor);
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

  stepperA.run();
  stepperB.run();
  stepperC.run();
  stepperD.run();
  stepperE.run();
  stepperF.run();
}

void schedule(AccelStepper &stepper, int task, const int sequence[], int seqPos) {
  // schedule again according to sequence
  scheduler.timer(task, sequence[seqPos % 7] * timeFactor);
  stepper.enableOutputs();
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.move(stepsPer90Degrees);
}

void doWhatNow() {
  // TODO repeat for all steppers
  //homing();
  
  for (int i=0; i < NUM_STEPPERS; i++) {
    if (steppers[i]->distanceToGo() == 0) {
      steppers[i]->disableOutputs();
    }
  }
  
  switch(scheduler.poll()) {
    case SEQ_A: 
      Serial.print("SEQ_A pos = ");
      Serial.println(seqAPos);
      
      schedule(stepperA, SEQ_A, sequenceA, seqAPos++); 
      //stopAOnMark = 1;
      break;

    case SEQ_B: 
      Serial.print("SEQ_B pos = ");
      Serial.println(seqBPos);
      
      schedule(stepperB, SEQ_B, sequenceB, seqBPos++); 
      break;

    case SEQ_C: 
      Serial.print("SEQ_C pos = ");
      Serial.println(seqCPos);
      
      schedule(stepperC, SEQ_C, sequenceC, seqCPos++); 
      break;

    case SEQ_D: 
      Serial.print("SEQ_D pos = ");
      Serial.println(seqDPos);
      
      schedule(stepperD, SEQ_D, sequenceD, seqDPos++); 
      break;

    case SEQ_E: 
      Serial.print("SEQ_E pos = ");
      Serial.println(seqEPos);
      
      schedule(stepperE, SEQ_E, sequenceE, seqEPos++); 
      break;

    case SEQ_F: 
      Serial.print("SEQ_F pos = ");
      Serial.println(seqFPos);
      
      // schedule again according to sequence
      schedule(stepperF, SEQ_F, sequenceF, seqFPos++); 
      break;

    case PRINT_TIME: 
      scheduler.timer(PRINT_TIME, 10);
      Serial.println(ms);
      break;
  }
  
}

void homing() {
  stopAOnMark = stepperA.distanceToGo() < 200;
  
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

