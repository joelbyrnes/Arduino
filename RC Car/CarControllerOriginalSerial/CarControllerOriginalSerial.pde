
/* digital pins */
#define PIN_PAN 9
#define PIN_TILT 10
#define PIN_FWD 6
#define PIN_REV 5
#define PIN_LEFT 8
#define PIN_RIGHT 7
#define PIN_SHOOT 12

/* analog pins */
#define PIN_V_CARBATT 0 

#define BUFSIZE 100

#define VDIV_R1 220
#define VDIV_R2 110

#include <Servo.h>

Servo tiltServo;
Servo panServo;

void setup() {
  pinMode(PIN_FWD, OUTPUT);
  pinMode(PIN_REV, OUTPUT);
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  pinMode(PIN_SHOOT, OUTPUT);
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
  digitalWrite(PIN_SHOOT, LOW);
  
  panServo.attach(PIN_PAN);
  tiltServo.attach(PIN_TILT);

  Serial.begin(38400);
  Serial.println("Controller ready");
}

void loop() {
  processSerial();
 
  sendData();
}  

long lastVRead = 0;

void sendData() {
  long now = millis();
  if ((now - lastVRead) > 1000) {
    long Vcc_mV = readVcc_mV();
    Serial.print("Vcc: ");
    Serial.println(Vcc_mV / 1000.0);
    float Vcar = getVoltage(Vcc_mV, PIN_V_CARBATT);
    Serial.print("Vcar: ");
    Serial.println(Vcar);
    lastVRead = now;
  }
}

float getVoltage(long Vcc_mV, int analogPin) {
  float raw = analogRead(analogPin);
  float Vin = (Vcc_mV / 1000.0 / 1023.0) * raw; // Calc Voltage with reference to Vcc
  float factor = (VDIV_R1 + VDIV_R2) / VDIV_R2;
  float Vcar = factor * Vin;
  return Vcar;
}

long readVcc_mV() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

char input[BUFSIZE];
int n;

void processSerial() {
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      Serial.println("overflow! discarding command. ");
      n = 0;
      break;
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
    } else if (strcmp(tok, "forward") == 0) {
      forward(value);
    } else if (strcmp(tok, "reverse") == 0) {
      reverse(value);
    } else if (strcmp(tok, "turn") == 0) {
      turn(value);
    } else if (strcmp(tok, "stop") == 0) {
      stopCar();
    } else if (strcmp(tok, "shoot") == 0) {
      shoot(value);
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

void forward(char value[]) {
  Serial.print("forward: ");
  if (value != NULL) {
    int pct = atoi(value);
    Serial.println(pct, DEC);
    goForward(pct);
  }
}

void reverse(char value[]) {
  Serial.print("reverse: ");
  if (value != NULL) {
    int pct = atoi(value);
    Serial.println(pct, DEC);
    goBack(pct);
  }
}

void turn(char value[]) {
  Serial.print("turn: ");
  if (value != NULL) {
    if (value[0] == 'l') {
      goLeft();
    } else if (value[0] == 'r') {
      goRight();
    } else {
      goStraight();
    }
  }
}


void goLeft() {
  digitalWrite(PIN_RIGHT, LOW);
  digitalWrite(PIN_LEFT, HIGH);
  Serial.println("left");
}

void goRight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, HIGH);
  Serial.println("right");
}

void goStraight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
  Serial.println("straight");
}

void goForward(int amount) {
  analogWrite(PIN_REV, 0);
  analogWrite(PIN_FWD, amount);
}

void goBack(int amount) {
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, amount);
}

void stopCar() {
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  Serial.println("stop");
}  

void shoot(char value[]) {
  Serial.print("shoot: ");
  if (value != NULL) {
    Serial.println(value);
    if (value[0] == '1') {
      digitalWrite(PIN_SHOOT, HIGH);
    } else {
      digitalWrite(PIN_SHOOT, LOW);
    }
  }
}


