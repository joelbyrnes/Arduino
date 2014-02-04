#include <SerialCommand.h>
#include <TimerOne.h>

int pin = 10;
int duty = 512;
long period = 1000;

SerialCommand sCmd;  

void setup() {
  Serial.begin(57600);
  
  pinMode(pin, OUTPUT);

  Timer1.initialize(); // init timer control library for pins 9 & 10

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("period",  readPeriod);          
  sCmd.addCommand("duty",   readDuty);       
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Timer1PWMSerial ready");
}

void loop() {
  serialInputLoop();
  //demoLoop();
}

void demoLoop() {
//  setFreq(261.63); 
//  delay(500);
}
  
void serialInputLoop() {
  if (Serial.available())
    sCmd.readSerial();  
}

void readDuty() {
  char *arg;
  arg = sCmd.next();    
  if (arg != NULL) {
    duty = atoi(arg);
    Serial.print("duty: ");
    Serial.println(duty);
    Timer1.pwm(pin, duty); 
  }
}

void readPeriod() {
  char *arg;
  arg = sCmd.next();    
  if (arg != NULL) {
    // turn off 
    Timer1.disablePwm(pin);
    digitalWrite(pin, 1); // inverted
    period = atol(arg);
    Serial.print("period: ");
    Serial.println(period);
    Timer1.pwm(pin, duty, period); 
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}

