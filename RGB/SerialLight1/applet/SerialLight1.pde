#include <Tlc5940.h>

struct rgb {
  byte r;
  byte g;
  byte b;
};

struct rgb colour;

void setup()
{
  Tlc.init();
  Serial.begin(9600);
  Serial.println("ready");
}

void loop() {
  byte r;
  byte g;
  byte b;
 
  struct rgb acolour = {0, 63, 255};  
  outputInverted(0, &acolour);
  Tlc.update();  
  printColour(&acolour);
  
  while (1) {
    r = readByte();
    g = readByte();
    b = readByte();
    colour = (struct rgb) {r, g, b};
    
    outputInverted(0, &colour);
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

// values are inverted because TLC is open collector output. 
void outputInverted(int light, struct rgb* rgb) {
  int portoffset = light * 3;
  Tlc.set(0 + portoffset, (rgb->r) * 16);
  Tlc.set(1 + portoffset, (rgb->g) * 16);
  Tlc.set(2 + portoffset, (rgb->b) * 16);
}
