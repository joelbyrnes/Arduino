// Automated Runner Lights (v1.0)
// Scott Martin & Todd Medema
// http://fabricate.io
// 10/3/2013
//
// Change this code to your heart's content!

// originally from http://www.instructables.com/id/Automatic-Hallway-Runner-Lights/step2/Program-the-Arduino/

#include <TimerOne.h>
// from Jeenode, for Sleepy timer. That library will not work with ATTiny85 so I'll have to pull it out or find another way. 
#include <Ports.h>

#define LOOP_DELAY 20
#define WAIT_TIME 500
#define HIBERNATE_TIME 20000
#define LED_PIN 10
#define PIR_PIN A3
#define LDR_POWER_PIN 12
#define LDR_INPUT_PIN A0
#define DEBUG false

// enables low power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// not sure how this works, turns off other interrupts so serial.print doesn't break sleep?
// http://forum.jeelabs.net/node/780
ISR(PCINT2_vect) {}

// Constants won't change. They're used here to set pin numbers and thresholds:
const int LIGHT_THRESH = 1000;
const int MOTION_THRESH = 100;
const int TIMEOUT = 1000;
const int pwmPeriod = 500; // in microseconds
const int maxBrightness = 512; // of 1024
const boolean invertPWM = true;

// variables will change:
int motionState = 0; // countdown after last motion
int lightState = 0; // countdown after last dark
int LEDActive = 0; // if LEDs are on
int LEDBrightness = 0; // current LED brightness (for fade in/out)

void setup() {
  Timer1.initialize(pwmPeriod); // init timer control library for pins 9 & 10

  // lights off to start with
  setLight(0);
  pinMode(LED_PIN, OUTPUT);

  // power LDR resistor on 12
  pinMode(LDR_POWER_PIN, OUTPUT);
  digitalWrite(LDR_POWER_PIN, 1);
  
  // set up input pins
  pinMode(LDR_INPUT_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
      
  if (DEBUG) {
    Serial.begin(57600);
    Serial.println("AutomaticNightLight debugging");
  }
}

void loop(){
  if (isLight()) {
    lightState = 0;
  } else {
    lightState = TIMEOUT;
  }
  
  if (lightState && motionDetected()) {
    motionState = TIMEOUT;
  }
 
  if (motionState > 0) motionState -= 1;
  if (lightState > 0) lightState -= 1;
 
  if (DEBUG) {
    Serial.print("cell: ");
    Serial.print(analogRead(LDR_INPUT_PIN));
    Serial.print(",\t pir: ");
    Serial.print(analogRead(PIR_PIN));
  }

  if (motionState && (lightState || LEDActive)) {    
    LEDActive = true;
  }
  else {
    LEDActive = false;
  }
  if (LEDActive) {
    if (LEDBrightness < maxBrightness) {
      LEDBrightness = min(LEDBrightness + 1, maxBrightness);
    }
  } else {
    if (LEDBrightness > 0) {
      LEDBrightness = max(LEDBrightness - 1, 0);
    }
  }
  
  // use parabola to bring up brightness more slowly at first
  // y = x^2 / maxBrightness
  int parabolicBrightness = int((double(LEDBrightness) * double(LEDBrightness)) / double(maxBrightness));

  if (DEBUG) {
    Serial.print(",\t motion: ");
    Serial.print(motionState);
    
    Serial.print(",\t light: ");
    Serial.print(lightState);
    
    Serial.print(",\t led: ");
    Serial.print(parabolicBrightness);
  }

  setLight(parabolicBrightness);

  digitalWrite(13, 0);
    
  // if we're active, small delay to bring up light
  if (LEDBrightness) {
    if (DEBUG) Serial.println(",\t mode: lit");
    delay(LOOP_DELAY);
    
    // if it's dark but not triggered, wait a bit
    // TODO replace this with an interrupt to wake from sleep
  } else if (lightState) {
    if (DEBUG) Serial.println(",\t mode: waiting");
    lowerPowerSleep(WAIT_TIME);
    
  // if it's bright, wait a while before checking again
  } else if (lightState == 0) {
    if (DEBUG) Serial.println(",\t mode: hibernating");
    lowerPowerSleep(HIBERNATE_TIME);
    // turn LED on for the time when power is being used, to indicate it is working in daylight.
    digitalWrite(13, 1);
  } else {
    if (DEBUG) Serial.println(",\t mode: wtf?");
  }
}

void lowerPowerSleep(long time) {
  // turn off LDR to save power
  digitalWrite(LDR_POWER_PIN, 0);
  
  // serial output doesn't work if we go to sleep, so we fake it without saving power
  if (DEBUG) delay(time);
  else Sleepy::loseSomeTime(time);
  
  digitalWrite(LDR_POWER_PIN, 1);
}

boolean isLight() {
  return analogRead(LDR_INPUT_PIN) < LIGHT_THRESH;
}

boolean motionDetected() {
  return analogRead(PIR_PIN) < MOTION_THRESH;
}

void setLight(int value) {
  if (value > maxBrightness) value = 1024;
  if (value < 0) value = 0;
  if (invertPWM) Timer1.pwm(LED_PIN, 1024 - value); 
  else Timer1.pwm(LED_PIN, value); 
}


