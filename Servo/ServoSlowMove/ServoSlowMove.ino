
//#include <Ports.h>
#include <JeeLib.h>

MilliTimer servoTimer; // in Ports.h

Servo servo;

void setup() {
    Serial.begin(112500);
    servo.attach(SOMEPIN);
}

int dir = 0;

void loop() {
  int minDeg = 60;
  int maxDeg = 120;
  int time = 1000;

  if (!running) {
    if (dir == 0) {
      move(minDeg, maxDeg, time);
      dir = 1;
    } else {
      move(maxDeg, minDeg, time);
      dir = 0;
    }  
    running = true;
  }
  
  checkMove();
}

int servoStartTime;
boolean running = false;

void move(int minDeg, int maxDeg, int time) {
  servoStartTime = millis();
  servo.write(minDeg);
}

void checkMove() {
  // only bother updating every X ms
  if (servoTimer.poll(30)) {
    int curTime = millis();
    // TODO check about max being less than min, negatives, etc
    int degDiff = maxDeg - minDeg;
    int timeElapsed = curTime - servoStartTime;
    
    float pos = minDeg + (timeElapsed / time) * degDiff 
    
    // TODO check time/pos has been reached, set running = false
    
    servo.write(pos);
  }
}
