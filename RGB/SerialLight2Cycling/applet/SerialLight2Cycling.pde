#include <Tlc5940.h>

#define REDPIN 4
#define GRNPIN 5
#define BLUPIN 6

struct rgb {
  byte r;
  byte g;
  byte b;
};

struct rgb colour;

void setup() {
  pinMode(REDPIN, OUTPUT); 
  pinMode(GRNPIN, OUTPUT); 
  pinMode(BLUPIN, OUTPUT);  

  Tlc.init();
  Serial.begin(9600);
  Serial.println("ready");
}

void loop() {
  testSequence();
}
  
void oneOutput() {
  Tlc.set(0, 4095);
  Tlc.update();  

  rgb(1, 1, 1);
  delay(1000);
}
  
void testSequence() {
//  struct rgb colour = {255, 0, 0};  
  
  showColour(4095, 0, 0, 200);
  showColour(0, 4095, 0, 200);
  showColour(0, 0, 4095, 200);
  showColour(4095, 0, 4095, 200);
}

void showColour(int red, int grn, int blu, int times) {
  for (int i = 0; i < times; i++) {
    setColour(red, grn, blu);
  }
}

void setColour(int red, int grn, int blu) {
  set(1, 0, 0, red);  
  set(0, 1, 0, grn);  
  set(0, 0, 1, blu);  
}

void set(int red, int grn, int blu, int value) {
  long microsec = 1000000;
  long wait = 1;

  Tlc.set(0, value);
  Tlc.update();  

  rgb(red, grn, blu);  
  delay(wait);
}

void rgb(int redOn, int grnOn, int bluOn) {
  digitalWrite(REDPIN, redOn);  
  digitalWrite(GRNPIN, grnOn);  
  digitalWrite(BLUPIN, bluOn);  
}
  
void old() {
  byte r;
  byte g;
  byte b;
 
  struct rgb acolour = {0, 63, 255};  
  output(0, &acolour);
  Tlc.update();  
  printColour(&acolour);
  
  while (1) {
    r = readByte();
    g = readByte();
    b = readByte();
    colour = (struct rgb) {r, g, b};
    
    output(0, &colour);
    Tlc.update();  
    printColour(&colour);
  }  
}

void printColour(struct rgb* rgb) {
    Serial.print("Colour: ");
    Serial.print(rgb->r, DEC); Serial.print(", ");
    Serial.print(rgb->g, DEC); Serial.print(", ");
    Serial.print(rgb->b, DEC); Serial.println();
}

// TODO this function will probably fill up the stack if it gets a lot of bad values. 
byte readByte() {
  while (Serial.available() <= 0) {
    delayMicroseconds(1000);
    Tlc.update();
  }
  int value = Serial.read();
  if (value != -1) {
    return byte(value);
  } else {
    return readByte();
  }
}

void output(int light, struct rgb* rgb) {
  int portoffset = light * 3;
  Tlc.set(0 + portoffset, rgb->r * 16);
  Tlc.set(1 + portoffset, rgb->g * 16);
  Tlc.set(2 + portoffset, rgb->b * 16);
}
