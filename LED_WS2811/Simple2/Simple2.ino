
#include "FastSPI_LED2.h"

#include <SerialCommand.h>

#define CONTROLPIN 11  

#define NUM_LEDS 240

#define USE_LEDS 30

struct CRGB leds[NUM_LEDS];

SerialCommand SCmd;   // The demo SerialCommand object

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(64);

  LEDS.addLeds<WS2811, CONTROLPIN>(leds, NUM_LEDS);
   
  LEDS.clear();
  
  Serial.begin(57600); 

  // Setup callbacks for SerialCommand commands 
  SCmd.addCommand("FOO",foo);     
  //SCmd.addCommand("ON",LED_on);       // Turns LED on
  //SCmd.addCommand("OFF",LED_off);        // Turns LED off
  SCmd.addCommand("HELLO",SayHello);     // Echos the string argument back
  //SCmd.addCommand("P",process_command);  // Converts two arguments to integers and echos them back 
  SCmd.addDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?") 
  
  Serial.println("Ready"); 

}

uint8_t val = 0;

void loop() { 
  SCmd.readSerial();     // We don't do much, just process serial commands

  for(int iLed = 0; iLed < NUM_LEDS; iLed++) {
    leds[iLed] = CRGB(0, 0, val); 
  }
  if (val < 255) val++;
  else val = 0;

  LEDS.show();
  delay(10);
 
  
}

void foo()
{
  Serial.println("Foo."); 
}

void SayHello()
{
  char *arg;  
  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL)      // As long as it existed, take it
  {
    Serial.print("Hello "); 
    Serial.println(arg); 
  } 
  else {
    Serial.println("Hello, whoever you are"); 
  }
}

// This gets set as the default handler, and gets called when no other command matches. 
void unrecognized()
{
  Serial.println("What?"); 
}

