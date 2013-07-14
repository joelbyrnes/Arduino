
#define BUFSIZE 100

#define PIN_PAN 15
#define PIN_TILT 14
#define forwardPin 17
#define reversePin 16
#define leftPin 10
#define rightPin 9

#include <Servo.h>

Servo tiltServo;
Servo panServo;

void setup()  
{
  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
}

void loop() {
  serialEcho();
}  

char input[BUFSIZE];
int n;

void serialEcho() {
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      Serial.println("overflow! discarding command. ");
    }
    char in = Serial.read();
    if (in != '\n') {
      input[n++] = in;
    } else {
      input[n] = '\0'; // end string
      processData(input);    
      n = 0;
    }
  }
}

void processData(char data[]) {
  char *tok = NULL;
  char *value = NULL;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "ping") == 0) {
      reply(value);
    } else if (strcmp(tok, "pan") == 0) {
      pan(value);
    } else if (strcmp(tok, "tilt") == 0) {
      tilt(value);
    } else if (strcmp(tok, "drive") == 0) {
      drive(value);
    } else {
      Serial.println("what?");
    }
    tok = strtok(NULL, " \t");
  }
}

void reply(char msg[]) {
  Serial.print("pong: ");
  if (msg != NULL) {
    Serial.println(msg);
  }
}

void pan(char value[]) {
  Serial.print("pan: ");
  if (value != NULL) {
    int pos = atoi(value);
    Serial.println(pos, DEC);
    // set pan... 
    panServo.write(pos);   
  }
}

void tilt(char value[]) {
  Serial.print("tilt: ");
  if (value != NULL) {
    int pos = atoi(value);
    Serial.println(pos, DEC);
    // set tilt... 
    tiltServo.write(pos);   
  }
}

void xdrive(char value[]) {
  Serial.print("drive: ");
  if (value != NULL) {
    int pos = atoi(value);
    Serial.println(pos, DEC);
    // set pan... 
    tiltServo.write(pos);   
  }
}




void drive(char value[]) {
  char fb = value[0];
  char lr = value[1];
  
  if (fb == 'f') {
    forward();
  } else if (fb == 'b') {
    back();
  } else {
    stopFB();
  }
  
  if (lr == 'l') {
    left();
  } else if (lr == 'r') {
    right();
  } else {
    stopLR();
  }
  
  delay(20);
}

void left() {
  digitalWrite(rightPin, LOW);
  digitalWrite(leftPin, HIGH);
  Serial.println("left");
}

void right() {
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, HIGH);
  Serial.println("right");
}

void forward() {
  digitalWrite(reversePin, LOW);
  digitalWrite(forwardPin, HIGH);
  Serial.println("forward");
}

void back() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, HIGH);
  Serial.println("reverse");
}

void stop() {
  stopFB();
  stopLR();
  Serial.println("stop all");
}  

void stopFB() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
  Serial.println("stop FB");
}

void stopLR() {
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
  Serial.println("stop LR");
}


