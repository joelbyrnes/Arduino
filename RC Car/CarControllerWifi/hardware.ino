
/* functions which access or change pins on the car to drive, shoot, etc */
  
void setupControlPins() {
#if defined(CONTROL_MODE_HBRIDGE)
  pinMode(PIN_FWD, OUTPUT);
  pinMode(PIN_REV, OUTPUT);
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
#endif
#if defined(CONTROL_MODE_SERVO)
  throttleServo.attach(PIN_THROTTLE);
  steerServo.attach(PIN_STEER);
#endif
    
  pinMode(PIN_SHOOT, OUTPUT);
  digitalWrite(PIN_SHOOT, LOW);

  panServo.attach(PIN_PAN);
  tiltServo.attach(PIN_TILT);
  
  initHardware();
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

// home all the servos and stop all motors
void initHardware() {
  stopCar();
  centerSteering();
  panServo.write(90); 
  tiltServo.write(80); // gun down

  // TODO put the brakes on?
}

void centerSteering() {
  // TODO use trim value for adjusting steering here?
  steerDegrees(90);
}

void pan(char value[]) {
  //debug("pan: ");
  if (value != NULL) {
    int pos = atoi(value);
    //debugln(pos, DEC);
    // set pan... 
    panServo.write(pos);   
  }
}

void tilt(char value[]) {
  //debug("tilt: ");
  if (value != NULL) {
    int pos = atoi(value);
    //debugln(pos, DEC);
    // set tilt... 
    tiltServo.write(pos);   
  }
}

void throttle(char value[]) {
  debug("forward: ");
  debugln(value);
  if (value != NULL) {
    // input value: 0 is full reverse, 127 is stop, 255 is full forward
    int amt = atoi(value);

#if defined(CONTROL_MODE_HBRIDGE)
    if (amt >= 127) {
      goForward((amt - 127) * 2);
    } else if (amt < 127) {
      goBack(255 - amt * 2);
    }
#endif    

#if defined(CONTROL_MODE_SERVO)
    // output: 0 degrees is full forward, stop is 90 degrees, 180 is full backwards
    int deg = map(amt, 255, 0, THROTTLE_MAX_REVERSE, THROTTLE_MAX_FORWARD);
    throttleServo.write(deg);
#endif
  }
}

void steer(char value[]) {
  //debug("steer: ");
  //debugln(value);
  if (value != NULL) {
    int pos = atoi(value);
    int servoPos = map(pos, 0, 255, STEER_MAX_RIGHT, STEER_MAX_LEFT);
    steerDegrees(servoPos);
  }
}

void steerDegrees(int pos) {
#if defined(CONTROL_MODE_HBRIDGE)
  if (pos > 90) {
    goLeft();
  } else if (pos < 88) {
    goRight();
  } else {
    goStraight(); 
  }
#endif    

#if defined(CONTROL_MODE_SERVO)
  // steer is based on servo values - straight 90, right 50, left 130
  steerServo.write(pos);   
#endif
  //debugln(pos, DEC);
}

#if defined(CONTROL_MODE_HBRIDGE)
void goLeft() {
  digitalWrite(PIN_RIGHT, LOW);
  digitalWrite(PIN_LEFT, HIGH);
  debugln("left");
}

void goRight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, HIGH);
  debugln("right");
}

void goStraight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
  //debugln("straight");
}

void goForward(int amount) {
  analogWrite(PIN_REV, 0);
  analogWrite(PIN_FWD, amount);
  //debugln("forward");
}

void goBack(int amount) {
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, amount);
  //debugln("back");
}
#endif

void stopCar() {
#if defined(CONTROL_MODE_HBRIDGE)
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
#endif    
#if defined(CONTROL_MODE_SERVO)
  throttleServo.write(90);
#endif    
  //debugln("stop");
}  

int shootValue = 0;

void shoot(char value[]) {
  //debug("shoot: ");
  if (value != NULL) {
    //debugln(value);
    if (value[0] == '1') {
      shootValue = 1;
      shootPin(HIGH);
      shootNormal(1);
    } else {
      shootNormal(0);
    }
  }
}

void shootPin(int value) {
  digitalWrite(PIN_SHOOT, value);
}

void generate38khzOnPins9And10() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Clear Timer on Compare Match (CTC) Mode
  bitWrite(TCCR1A, WGM10, 0);
  bitWrite(TCCR1A, WGM11, 0);
  bitWrite(TCCR1B, WGM12, 1);
  bitWrite(TCCR1B, WGM13, 0);

  // Toggle OC1A and OC1B on Compare Match.
  bitWrite(TCCR1A, COM1A0, 1);
  bitWrite(TCCR1A, COM1A1, 0);
  bitWrite(TCCR1A, COM1B0, 1);
  bitWrite(TCCR1A, COM1B1, 0);

  // No prescaling
  bitWrite(TCCR1B, CS10, 1);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS12, 0);

  OCR1A = 210;
  OCR1B = 210;

  Serial.println("Generating 38kHz signal on pins 9 & 10");
}

