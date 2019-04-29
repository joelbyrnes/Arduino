/*
  Program for Scott Moorhead's "zen" board

A :  3   4   5   6   7   8   3   (repeat)   36
B :  4   5   6   7   8   3   4   (repeat)   37
C :  5   6   7   8   3   4   5   (repeat)   38
D :  6   7   8   3   4   5   6   (repeat)   39
E :  7   8   3   4   5   6   7   (repeat)   40
F :  8   3   4   5   6   7   8   (repeat)   41 

Move 90 degrees after x seconds/minutes. 
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

#define PAUSE_PIN    8

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
// HALF4WIRE or FULL4WIRE
AccelStepper stepperA(AccelStepper::HALF4WIRE, motorPinA1, motorPinA3, motorPinA2, motorPinA4);
AccelStepper stepperB(AccelStepper::HALF4WIRE, motorPinB1, motorPinB3, motorPinB2, motorPinB4);
AccelStepper stepperC(AccelStepper::HALF4WIRE, motorPinC1, motorPinC3, motorPinC2, motorPinC4);
AccelStepper stepperD(AccelStepper::HALF4WIRE, motorPinD1, motorPinD3, motorPinD2, motorPinD4);
AccelStepper stepperE(AccelStepper::HALF4WIRE, motorPinE1, motorPinE3, motorPinE2, motorPinE4);
AccelStepper stepperF(AccelStepper::HALF4WIRE, motorPinF1, motorPinF3, motorPinF2, motorPinF4);

#define NUM_STEPPERS 6

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

#define MAX_SPEED  1400.0     // steps per second
#define ACCEL 5000.0
#define SPEED 1400.0
int stepsPer90Degrees = 1019;

const int lightThreshold = 100;

int stopAOnMark;
int stopFOnMark;
int foundA;
int foundF;

enum {SEQ_A, SEQ_B, SEQ_C, SEQ_D, SEQ_E, SEQ_F, PRINT_TIME, TASK_LIMIT};

Scheduler scheduler (TASK_LIMIT);

struct StepperCluster {
  AccelStepper *stepper;
  const int sequence[7];
  int seqPos;
  int task;
  const int sensorPin;
  const int ledPin;
  boolean seeking;
  boolean found;
}; 

StepperCluster clusterA { &stepperA, {3, 4, 5, 6, 7, 8, 3}, 0, SEQ_A, A1, 8,  0, 0 };
StepperCluster clusterB { &stepperB, {4, 5, 6, 7, 8, 3, 4}, 0, SEQ_B, A2, 9,  0, 0 };
StepperCluster clusterC { &stepperC, {5, 6, 7, 8, 3, 4, 5}, 0, SEQ_C, A3, 10, 0, 0 };
StepperCluster clusterD { &stepperD, {6, 7, 8, 3, 4, 5, 6}, 0, SEQ_D, A4, 11, 0, 0 };
StepperCluster clusterE { &stepperE, {7, 8, 3, 4, 5, 6, 7}, 0, SEQ_E, A5, 12, 0, 0 };
StepperCluster clusterF { &stepperF, {8, 3, 4, 5, 6, 7, 8}, 0, SEQ_F, A6, 13, 0, 0 };

StepperCluster *clusters[NUM_STEPPERS] = {&clusterA, &clusterB, &clusterC, &clusterD, &clusterE, &clusterF}; 

// the time between sequence steps, in seconds.
int timeFactor = 4;

// reverse direction after this many cycles
#define CYCLE_REVERSE_COUNT 25

unsigned int ms;

void setup() {
  Serial.begin(57600); 

  scheduler.timer(PRINT_TIME, 10);
  
  for (int i=0; i < NUM_STEPPERS; i++) {
    // TODO?
    //pinMode(ledPin, OUTPUT);
    //digitalWrite(ledPin, LOW);

    // configure steppers
    clusters[i]->stepper->setMaxSpeed(MAX_SPEED);
    clusters[i]->stepper->setAcceleration(ACCEL);
    clusters[i]->stepper->setSpeed(SPEED);  
    // start off disabled 
    clusters[i]->stepper->disableOutputs();
    
    pinMode(clusters[i]->ledPin, OUTPUT);    
    digitalWrite(clusters[i]->ledPin, LOW);
    
    // initially look for the home marker
    //clusters[i]->seeking = 1;

    // schedule movements in tenths of a second, ie 5 = 0.5s
    scheduler.timer(clusters[i]->task, (clusters[i]->sequence[clusters[i]->seqPos]) * timeFactor * 10);
    clusters[i]->seqPos++;
  }
}

boolean paused() {
  // paused when connected to ground, otherwise floating 
  return analogRead(PAUSE_PIN) == 0;
}

void loop() {
  ms = millis();
  
  /* for homing which is not currently used
  for (int i=0; i < NUM_STEPPERS; i++) {
    int lightValue = analogRead(clusters[i]->sensorPin);
  
    // light up LED if we have found the mark
    if (lightValue > lightThreshold) {
      digitalWrite(clusters[i]->ledPin, HIGH);
      clusters[i]->found = 1;
    } else {
      digitalWrite(clusters[i]->ledPin, LOW);
      clusters[i]->found = 0;
    }
  }
  */
  
  doWhatNow();
  
  stepperA.run();
  stepperB.run();
  stepperC.run();
  stepperD.run();
  stepperE.run();
  stepperF.run();
}

void schedule(struct StepperCluster &cluster) {
  // schedule again according to sequence
  scheduler.timer(cluster.task, cluster.sequence[cluster.seqPos % 7] * timeFactor * 10);

  Serial.print("paused = ");
  Serial.println(paused());

  // don't make movements if pause button on; pattern will proceed but without movement. 
  if (paused()) return;

  cluster.seqPos++;
  cluster.stepper->enableOutputs();
  cluster.stepper->setMaxSpeed(MAX_SPEED);
  // reverse direction after each cycle through, to avoid slight rotation drift over time 
  int steps = (cluster.seqPos / 7 * CYCLE_REVERSE_COUNT) % 2? stepsPer90Degrees * -1: stepsPer90Degrees;
  cluster.stepper->move(steps);
  cluster.seeking = 1;
}

void doWhatNow() {
  for (int i=0; i < NUM_STEPPERS; i++) {
    // save power by turning off outputs when not moving
    if (clusters[i]->stepper->distanceToGo() == 0) {
      clusters[i]->stepper->disableOutputs();
    }
    
    // TODO 
    //homing();
  }
  
  switch(scheduler.poll()) {
    case PRINT_TIME: 
      scheduler.timer(PRINT_TIME, 10);
      Serial.println(ms);
      break;

    case SEQ_A: 
      Serial.print("SEQ_A pos = ");
      Serial.println(clusterA.seqPos % 7);
      
      schedule(clusterA); 
      break;

    case SEQ_B: 
      Serial.print("SEQ_B pos = ");
      Serial.println(clusterB.seqPos % 7);
      
      schedule(clusterB); 
      break;

    case SEQ_C: 
      Serial.print("SEQ_C pos = ");
      Serial.println(clusterC.seqPos % 7);
      
      schedule(clusterC); 
      break;

    case SEQ_D: 
      Serial.print("SEQ_D pos = ");
      Serial.println(clusterD.seqPos % 7);
      
      schedule(clusterD); 
      break;

    case SEQ_E: 
      Serial.print("SEQ_E pos = ");
      Serial.println(clusterE.seqPos % 7);
      
      schedule(clusterE); 
      break;

    case SEQ_F: 
      Serial.print("SEQ_F pos = ");
      Serial.println(clusterF.seqPos % 7);
      
      schedule(clusterF); 
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

