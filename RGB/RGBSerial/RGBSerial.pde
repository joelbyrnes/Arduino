
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

struct rgb {
  byte r;
  byte g;
  byte b;
}; 

void setup() {
  pinMode(8, OUTPUT);  
  digitalWrite(8, 0);
  Serial.begin(57600);
  Serial.println("ready");
}

void loop() {
  //test();
  doSerialThing();
}

void test() {
  struct rgb acolour = {255, 0, 0};  
  struct rgb bcolour = {0, 255, 0};  
  struct rgb ccolour = {0, 0, 255};  
  
  output(&acolour);
  printColour(&acolour);
  delay(1000);

  output(&bcolour);
  printColour(&bcolour);
  delay(1000);

  output(&ccolour);
  printColour(&ccolour);
  delay(1000);
}
  
void doSerialThing() {
  byte r;
  byte g;
  byte b;
  
  struct rgb acolour = {0, 0, 0};  
  
  if (Serial.available() > 4) {
    // sync up to 100, 10 first
    if (Serial.read() == 100 && Serial.read() == 10) {
      r = Serial.read();
      g = Serial.read();
      b = Serial.read();
  
      acolour = (struct rgb) {r, g, b};
      
      output(&acolour);
      printColour(&acolour);
    }
  }  
}

boolean sync() {
  return (Serial.read() == 100 && Serial.read() == 10);
}

void printColour(struct rgb* rgb) {
    Serial.print("Colour: ");
    Serial.print(rgb->r, DEC); Serial.print(", ");
    Serial.print(rgb->g, DEC); Serial.print(", ");
    Serial.print(rgb->b, DEC); Serial.println();
}

void output(struct rgb* rgb) {
  analogWrite(redPin, rgb->r);
  analogWrite(grnPin, rgb->g);
  analogWrite(bluPin, rgb->b);
}

