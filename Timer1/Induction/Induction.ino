
#include <SerialCommand.h>
#include <TimerOne.h>

int pwmPin = 10;
int ledPin = 13;

SerialCommand sCmd;     // The demo SerialCommand object

void setup() {
  Serial.begin(57600);
  
  pinMode(pwmPin, OUTPUT);
  
  Timer1.initialize(); // init timer control library for pins 9 & 10
  
  Timer1.pwm(pwmPin, 512, 2); 

  // Setup callbacks for SerialCommand commands
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Induction ready");
}

void loop() {
  //test();
}

void test() {
  Timer1.pwm(pwmPin, 256, 1); 
  delay(500);
  Timer1.pwm(pwmPin, 383, 1); 
  delay(500);
  Timer1.pwm(pwmPin, 383, 2); 
  delay(500);

  
  Timer1.disablePwm(pwmPin);
  delay(1000);
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}

