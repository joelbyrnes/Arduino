#include <JeeLib.h>

const int led1Pin = 2;
const int led2Pin = 3;
const int led3Pin = 4;

enum {LED1, LED1_OFF, LED2, LED2_OFF, LED3, LED3_OFF, PRINT_TIME, TASK_LIMIT};

Scheduler scheduler (TASK_LIMIT);

int sequence1[7] = {3, 4, 5, 6, 7, 8, 3};
int seq1Pos = 0;

void setup() {
  pinMode(led1Pin, OUTPUT);    
  pinMode(led2Pin, OUTPUT);    
  pinMode(led3Pin, OUTPUT);    
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  
  // tenths of a second, ie 5 = 0.5s
  scheduler.timer(LED1, sequence1[seq1Pos++] * 2);
  scheduler.timer(LED2, 10);
  scheduler.timer(LED3, 15);
  scheduler.timer(PRINT_TIME, 10);
    
  Serial.begin(57600); 
}

void loop() {
  unsigned long ms = millis();

  switch(scheduler.poll()) {
    case LED1: 
      // turn it off in 0.5s and schedule on again according to sequence
      scheduler.timer(LED1_OFF, 2);
      scheduler.timer(LED1, sequence1[seq1Pos++] * 2);
      digitalWrite(led1Pin, HIGH);
      // reset back to start
      if (seq1Pos == 7) seq1Pos = 0;
      break;
      
    case LED2: 
      digitalWrite(led2Pin, HIGH);
      scheduler.timer(LED2_OFF, 5);
      scheduler.timer(LED2, 10);
      break;
      
    case LED3: 
      digitalWrite(led3Pin, HIGH);
      scheduler.timer(LED3_OFF, 5);
      scheduler.timer(LED3, 10);
      break;

    case LED1_OFF: 
      digitalWrite(led1Pin, LOW);
      break;

    case LED2_OFF: 
      digitalWrite(led2Pin, LOW);
      break;

    case LED3_OFF: 
      digitalWrite(led3Pin, LOW);
      break;

    case PRINT_TIME: 
      scheduler.timer(PRINT_TIME, 10);
      Serial.println(ms);
      break;
  }

}

