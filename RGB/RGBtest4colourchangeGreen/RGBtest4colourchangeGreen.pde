
int redPin = 2;
int grnPin = 3;
int bluPin = 4;

void setup()                   
{
  pinMode(redPin, OUTPUT); 
  pinMode(grnPin, OUTPUT); 
  pinMode(bluPin, OUTPUT);  
  Serial.begin(9600);
}

void loop()       
{
  int time = 800;
  
  // red to green
//  fadeTo(255, 0, 0, 0, 255, 0, time); 
  // green to white
  fadeTo(0, 255, 0, 255, 255, 255, time); 
  fadeTo(255, 255, 255, 255, 255, 255, time); 
  // white to green
  fadeTo(255, 255, 255, 0, 255, 0, time); 
  fadeTo(0, 255, 0, 0, 255, 0, time); 
  // green to black
  fadeTo(0, 255, 0, 0, 0, 0, time); 
  fadeTo(0, 0, 0, 0, 0, 0, time); 
  // black to green
  fadeTo(0, 0, 0, 0, 255, 0, time); 
  fadeTo(0, 255, 0, 0, 255, 0, time); 
  
  // g to b
//  fadeTo(0, 255, 0, 0, 0, 255, time); 
  // b to r
//  fadeTo(0, 0, 255, 255, 0, 0, time);

}

void logLong(char* label, unsigned long num) {
  Serial.print(label);
  Serial.print(num);
  Serial.print("\n");
}

// fade from one RGB to another in steps, like percentage
void fadeTo(int red1, int grn1, int blu1, int red2, int grn2, int blu2, int steps) {
  int red = red1;
  int grn = grn1;
  int blu = blu1;  
  
  for (int i = 0; i < steps; i++) {
    double percent = double(i)/double(steps);
    red = red1 + ( (red2 - red1) * percent );
    grn = grn1 + ( (grn2 - grn1) * percent );
    blu = blu1 + ( (blu2 - blu1) * percent );
    
    colour(red, grn, blu);
  }
}

// reduce the colour to 0 in steps, like percentage
void fadeDown(int red, int grn, int blu, int steps) {
  fadeTo(red, grn, blu, 0, 0, 0, steps);
}

// increase the colour from 0 to R G B in steps, like percentage
void fadeUp(int red, int grn, int blu, int steps) {
  fadeTo(0, 0, 0, red, grn, blu, steps);
}

// do 256 iterations, given different values for each of R G B until they are "used up"
void colour(int red, int grn, int blu) {
  for (int i = 0; i < 256; i++) {
    // if colour is greater than 0, turn it on, else off, then decrement.
    rgb((red-- > 0), (grn-- > 0), (blu-- > 0));
  }
}

void rgb(int redOn, int grnOn, int bluOn) {
  digitalWrite(redPin, redOn);  
  digitalWrite(grnPin, grnOn);  
  digitalWrite(bluPin, bluOn);  
}
