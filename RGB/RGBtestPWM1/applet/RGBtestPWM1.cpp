
#include "WProgram.h"
void setup();
void loop();
void fadeTo(int red1, int grn1, int blu1, int red2, int grn2, int blu2, int time);
void colour(int red, int grn, int blu);
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

void setup() {
  //Serial.begin(9600);
}

void loop() {
  int time = 800;
  
  // red to green
  fadeTo(255, 0, 0, 0, 255, 0, time); 
  delay(time);
  // green to white
  fadeTo(0, 255, 0, 255, 255, 255, time); 
  delay(time);
  // white to green
  fadeTo(255, 255, 255, 0, 255, 0, time); 
  delay(time);
  // green to black
  fadeTo(0, 255, 0, 0, 0, 0, time); 
  delay(time);
  // black to green
  fadeTo(0, 0, 0, 0, 255, 0, time); 
  delay(time);
  
  // g to b
  fadeTo(0, 255, 0, 0, 0, 255, time); 
  delay(time);
  // b to r
  fadeTo(0, 0, 255, 255, 0, 0, time);
  delay(time);
}

// fade from one RGB to another in steps, like percentage
void fadeTo(int red1, int grn1, int blu1, int red2, int grn2, int blu2, int time) {
  int steps = 100;
  int stepDelay = time / steps;  
  
  int red;
  int grn;
  int blu;  
  
  for (int istep = 0; istep < steps; istep++) {
    double percent = double(istep)/double(steps - 1);
    red = red1 + ( (red2 - red1) * percent );
    grn = grn1 + ( (grn2 - grn1) * percent );
    blu = blu1 + ( (blu2 - blu1) * percent );
    
    colour(red, grn, blu);
    delay(stepDelay);
  }
}

void colour(int red, int grn, int blu) {
  analogWrite(redPin, red);
  analogWrite(grnPin, grn);
  analogWrite(bluPin, blu);
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

