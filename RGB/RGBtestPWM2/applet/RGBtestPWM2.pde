
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

struct rgb {
  byte r;
  byte g;
  byte b;
}; 

struct light {
  int rPin;
  int gPin;
  int bPin;
  struct rgb rgb;
}; 

void setup() {
  //Serial.begin(9600);
}

void loop() {
  rbFade();
  colourtest();
}

void rbFade() {
  struct rgb rgbR = {255, 0, 0};
  struct rgb rgbB = {0, 0, 255};
  struct light light = {redPin, grnPin, bluPin, rgbR};
  delay(500);
  fadeTo(&light, rgbB, 100);
  delay(500);
  fadeTo(&light, rgbR, 100);
}

void colourtest() {
  int time = 800;
  
  // red to green
  fadeTo(255, 0, 0, 0, 255, 0, time); 
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
  fadeTo(0, 255, 0, 0, 0, 255, time); 
  // b to r
  fadeTo(0, 0, 255, 255, 0, 0, time);
}

// fade from one RGB to another in steps, like percentage
void fadeTo(int red1, int grn1, int blu1, int red2, int grn2, int blu2, int time) {
  struct rgb rgb1 = {red1, grn1, blu1};
  struct rgb rgb2 = {red2, grn2, blu2};
  struct light light = {redPin, grnPin, bluPin, rgb1};
  fadeTo(&light, rgb2, time);
}
 
// fade from one RGB to another in steps, like percentage
void fadeTo(struct light* light, struct rgb rgb2, int time) {
  int steps = 100;
  int stepDelay = (time / steps);

  struct rgb rgb1 = {light->rgb.r, light->rgb.g, light->rgb.b}; // save original rgb 
  for (int i = 0; i < steps; i++) {
    double percent = double(i)/double(steps - 1);
    light->rgb.r = rgb1.r + ( (rgb2.r - rgb1.r) * percent );
    light->rgb.g = rgb1.g + ( (rgb2.g - rgb1.g) * percent );
    light->rgb.b = rgb1.b + ( (rgb2.b - rgb1.b) * percent );
    
    output(light);
    delay(stepDelay);
  }
}

void output(struct light* light) {
  analogWrite(light->rPin, light->rgb.r);
  analogWrite(light->gPin, light->rgb.g);
  analogWrite(light->bPin, light->rgb.b);
}
