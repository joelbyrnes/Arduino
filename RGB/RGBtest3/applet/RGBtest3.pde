
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
  unsigned long start;
  unsigned long end;
 
  start = micros(); 
  fadeDown(0, 50, 255, 100);
  end = micros();
  rgb(0,0,0);
  logLong("fadeDown: ", end - start);

  delay(500);
  
  //fadeUp(255, 100, 25, 100);
  start = micros(); 
  fadeTo(0, 50, 255, 0, 0, 0, 100);
  end = micros();
  rgb(0,0,0);
  logLong("fadeTo:   ", end - start);
  
  start = micros(); 
  delay(500);
  end = micros();
  logLong("delay:    ", end - start);
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
