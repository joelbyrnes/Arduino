
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

void setup() {
  pinMode(8, OUTPUT);  
  digitalWrite(8, 1);

  colour(128, 128, 128);
  Serial.begin(115200);

}

void loop() {

}

void colour(byte red, byte grn, byte blu) {
  analogWrite(redPin, red);
  analogWrite(grnPin, grn);
  analogWrite(bluPin, blu);
}


