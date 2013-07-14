
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

int addrPin0 = 2;
int addrPin1 = 3;
int addrPin2 = 4;
int addrPin3 = 5;

struct rgb {
  byte r;
  byte g;
  byte b;
}; 

struct light {
  int address;
  struct rgb rgb;
  struct rgb rgb2;
}; 

int NUMLIGHTS = 4;

struct light lights[4];

void setup() {
  pinMode(addrPin0, OUTPUT); 
  pinMode(addrPin1, OUTPUT); 
  pinMode(addrPin2, OUTPUT); 
  pinMode(addrPin3, OUTPUT); 

  //Serial.begin(9600);
}

void loop() {
  //rbgFade();
  //rSwap();
  testAddressing();
}

void testAddressing() {
  struct rgb black = {0, 0, 0};
  struct rgb rgbR = {255, 0, 0};
  struct rgb rgbG = {0, 255, 0};
  struct rgb rgbB = {0, 0, 255};
  
  struct rgb fade1 = {255, 0, 0};
  struct rgb fade2 = {0, 255, 0};

  int i;
  while (1) {
    for (i = 0; i < 256; i += 1) {
      fade1.r = i;
      fade1.g = 256 - i;
      select(3); // don't look, changing colour!
      pwm(&fade1);
      delayMicroseconds(2050); // at 488hz, pwm output is 20491us per wave
      //delay(1);
      select(1); // output to light 0
      delayMicroseconds(200000);
      //delay(50);
      /*            
      fade2.g = i;
      fade2.b = 256 - i;
      select(3); // don't look, changing colour!
      pwm(&fade2);
      delayMicroseconds(2050); // at 488hz, pwm output is 20491us per wave
      //delay(1);
      select(1); // output to light 1
      delayMicroseconds(200000);
      //delay(50);
      */
    }
  }
}
/*
void rSwap() {
  struct rgb rgbR = {255, 0, 0};
  struct rgb black = {0, 0, 0};
  lights[0] = (struct light) {0, (struct rgb) {255, 0, 0}, (struct rgb) {0, 0, 0}};
  lights[1] = (struct light) {1, (struct rgb) {0, 0, 0}, (struct rgb) {255, 0, 0});
  delay(500);
  transition(lights, 100);
  delay(500);
  lights[0].rgb2 = (struct rgb) {255, 0, 0};
  lights[1].rgb2 = (struct rgb) {0, 255, 0};
  transition(lights, 100);
}
*/
void rbgFade() {
  struct rgb black = {0, 0, 0};
  struct rgb rgbR = {255, 0, 0};
  struct rgb rgbG = {0, 255, 0};
  struct rgb rgbB = {0, 0, 255};
  lights[0] = (struct light) {0, rgbR, rgbG};
  lights[1] = (struct light) {1, rgbG, rgbB};
  transition(lights, 100);
  delay(500);
  lights[0].rgb2 = rgbB;
  lights[1].rgb2 = rgbR;
  transition(lights, 100);
  delay(500);
  lights[0].rgb2 = rgbR;
  lights[1].rgb2 = rgbG;
  transition(lights, 100);
  delay(500);
}

// fade from one RGB to another in steps, like percentage
void transition(struct light lights[], int time) {
  int steps = 100;
  int stepDelay = (time / steps);

  // save original RGBs  
  struct rgb origRgb[4] = {lights[0].rgb}; // TODO 
  
  struct rgb rgb2 = lights[0].rgb2;
  for (int i = 0; i < steps; i++) {
    increment(&lights[0].rgb, &origRgb[0], &lights[0].rgb2, i, steps);
    //increment(&lights[1].rgb, &origRgb[1], &lights[1].rgb2, i, steps);
    output(&lights[0]);
    //output(&lights[1]);
    
    delay(stepDelay);
  }
}

void increment(struct rgb* rgb, struct rgb* rgb1, struct rgb* rgb2, int step, int steps) {
  double percent = double(step)/double(steps - 1);
  rgb->r = rgb1->r + ( (rgb2->r - rgb1->r) * percent );
  rgb->g = rgb1->g + ( (rgb2->g - rgb1->g) * percent );
  rgb->b = rgb1->b + ( (rgb2->b - rgb1->b) * percent );
}

void output(struct light* light) {
  select(light->address);
  pwm(&light->rgb);
}

void select(int address) {
  // todo set address properly
  if (address == 0) {
    digitalWrite(addrPin0, 0);
    digitalWrite(addrPin1, 0);
  } else if (address == 1) {
    digitalWrite(addrPin0, 1);
    digitalWrite(addrPin1, 0);
  } else if (address == 2) {
    digitalWrite(addrPin0, 0);
    digitalWrite(addrPin1, 1);
  } else if (address == 3) {
    digitalWrite(addrPin0, 1);
    digitalWrite(addrPin1, 1);
  }
}

void pwm(struct rgb* rgb) {
  analogWrite(redPin, rgb->r);
  analogWrite(grnPin, rgb->g);
  analogWrite(bluPin, rgb->b);
}
