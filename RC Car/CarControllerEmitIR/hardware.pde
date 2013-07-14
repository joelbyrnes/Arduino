
/* functions which access or change pins on the car to drive, shoot, etc */

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
  panServo.write(90); 
  tiltServo.write(65); // gun down
  steerDegrees(90);

  // TODO put the brakes on?
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
      goBack(amt * 2);
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
  } else if (pos < 90) {
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

void shoot(char value[]) {
  //Serial.print("shoot: ");
  if (value != NULL) {
    //Serial.println(value);
    if (value[0] == '1') {
      digitalWrite(PIN_SHOOT, HIGH);
    } else {
      digitalWrite(PIN_SHOOT, LOW);
    }
  }
}

