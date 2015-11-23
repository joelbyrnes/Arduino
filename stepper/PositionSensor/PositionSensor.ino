
#include <AccelStepper.h>

// Motor pin definitions
#define motorPin1  10     // IN1 on the ULN2003 driver 1
#define motorPin2  11     // IN2 on the ULN2003 driver 1
#define motorPin3  12     // IN3 on the ULN2003 driver 1
#define motorPin4  13     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

// according to http://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/ 
// this motor isn't quite 4096 steps, more like 4076. 1019 per 90 degrees.

const int stepsPer90Degrees = 1019;
const int sensorPin = A1;    // pin that the sensor is attached to
const int lightThreshold = 100;
const int ledPin = 8;

void setup() {
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);

  Serial.begin(57600); 
    
  delay(100);
  
  // TODO home the steppers first
  
  stepper1.setMaxSpeed(1200.0);
  stepper1.setAcceleration(800.0);
  stepper1.setSpeed(400);
}

int lightValue = 0;
int stepsWithLight = 0;
int stepCount = 0;
int found = 0;

void loop() {

  if (!found) {
    if (stepper1.distanceToGo() == 0) {
      int lightValue = analogRead(sensorPin);
  
      Serial.println(lightValue);  
  
      if (lightValue > lightThreshold) {
        digitalWrite(ledPin, HIGH);
        found = 1;
        Serial.print("steps to find = ");  
        Serial.println(stepCount);  
        
      } else {
        digitalWrite(ledPin, LOW);
      }
      
      stepper1.move(1);
      stepCount++;
    }
  }

  stepper1.run();
}

void findStartPosition() {
  
}

