// a4 = 440Hz
// 1000000 / 440 = 2272.72
// experimentally determined that 2258 works so there is probably approx 14us in the loop
// also C4 = 261Hz, which is 3819 us, but 3805 works
#define LOOP_DELAY 24

#include <SerialCommand.h>
#include <TimerOne.h>

int dirpin = 8;
int steppin = 9;

long delayus = 0; // minimum 100 with microstepping

SerialCommand sCmd;     // The demo SerialCommand object

void setup() {
  Serial.begin(57600);
  
  pinMode(dirpin, OUTPUT);
  pinMode(steppin, OUTPUT);

  digitalWrite(dirpin, LOW);     // Set the direction.
  
  Timer1.initialize(); // init timer control library for pins 9 & 10

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("delay",  readDelay);          // Turns LED on
  sCmd.addCommand("freq",   readFreq);         // Turns LED off
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("StepperMusicTimer1 ready");
}

void loop() {
  serialInputLoop();
  //demoLoop();
}

void demoLoop() {
  setFreq(261.63); // C4
  delay(500);
  setFreq(293.66); // D4
  delay(500);
  setFreq(329.63); // E4
  delay(500);
  setFreq(349.23); // F4
  delay(500);
  setFreq(392.00); // G4
  delay(500);
  setFreq(440.00); // A4
  delay(500);
  setFreq(493.88); // B4
  delay(500);
  setFreq(523.25); // C5
  delay(500);
}
  
void serialInputLoop() {
  if (Serial.available())
    sCmd.readSerial();  
}

void readDelay() {
  char *arg;
  arg = sCmd.next();    
  if (arg != NULL) {
    long value = atol(arg);
    Serial.print("delay: ");
    Serial.println(value);
    delayus = value;
    Timer1.pwm(steppin, 512, value); // set pwm wave on steppin, 50% duty cycle (of 1024), period in microseconds.
  }
}

void readFreq() {
  char *arg;
  arg = sCmd.next();    
  if (arg != NULL) {
    float value = atof(arg);
    Serial.print("freq: ");
    Serial.println(value);
    setFreq(value);
  }
}

void setFreq(double freq) {
  if (freq <= 0.0) delayus = 0;
  else delayus = long(1000000.0 / freq);
  Serial.print("delay: ");
  Serial.println(delayus);
  Timer1.pwm(steppin, 512, delayus); // set pwm wave on steppin, 50% duty cycle (of 1024), period in microseconds.
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}

