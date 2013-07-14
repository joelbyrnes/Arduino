
#include "FastSPI_LED2.h"

#include <SerialCommand.h>

#define CONTROLPIN 11

#define NUM_LEDS 240

#define USE_LEDS 240

struct CRGB leds[NUM_LEDS];

SerialCommand SCmd;   // The demo SerialCommand object

// function pointer which can be changed by serial commands
// cannot take any arguments, like a creationist. 
void (*current_function)() = NULL;

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(64);

  LEDS.addLeds<WS2811, CONTROLPIN, GRB>(leds, NUM_LEDS);
   
  LEDS.clear();
  
  Serial.begin(57600); 

  // Setup callbacks for SerialCommand commands 
  SCmd.addCommand("RED", red);     
  SCmd.addCommand("GRN", green);     
  SCmd.addCommand("BLU", blue);  
  SCmd.addCommand("PRETTY", pretty);  
  
  SCmd.addCommand("HELLO",SayHello);     // Echos the string argument back
  //SCmd.addCommand("P",process_command);  // Converts two arguments to integers and echos them back 
  SCmd.addDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?") 
  
  Serial.println("Ready"); 
  
  current_function = rainbow;
}

void loop() { 
  if (Serial.available())
    SCmd.readSerial();     // We don't do much, just process serial commands
  
  if (current_function != NULL) {
     current_function();
  }
}

void red() {
  Serial.println("red!"); 
  LEDS.showColor(CRGB(255, 0, 0));
  current_function = NULL; 
}

void green() {
  Serial.println("green!"); 
  LEDS.showColor(CRGB(0, 255, 0));
  current_function = NULL; 
}

void blue() {
  Serial.println("blue!"); 
  LEDS.showColor(CRGB(0, 0, 255));
  current_function = NULL; 
}

void pretty() {
  Serial.println("pretty rainbow!"); 
  // TODO set rainbow as function to call in loop
  current_function = rainbow; 
}

void rainbow() {
  //Serial.println("doing rainbow");
  //for(int i=0; i < 200; i++) {rainbow_loop(10, 20);}
  rainbow_loop(10, 20);
}

int idex = 0;        //-LED INDEX (0 to NUM_LEDS-1
int ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)

void rainbow_loop(int istep, int idelay) { //-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + istep;
  int icolor[3];  

  if (idex >= NUM_LEDS) {idex = 0;}
  if (ihue >= 359) {ihue = 0;}

  //HSVtoRGB(ihue, 255, 255, icolor);
  //leds[idex] = CRGB(icolor[0], icolor[1], icolor[2]); 
  
  CHSV rainbowcolor;
  rainbowcolor.hue =              ihue;
  rainbowcolor.saturation =       255;
  rainbowcolor.value =            255;
  hsv2rgb_rainbow( rainbowcolor, leds[idex] );
  
  LEDS.show();
  delay(idelay);
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

