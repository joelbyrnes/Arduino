
int ledPin = 13;
int analogPin = 0;     // voltage-divider circuit attached to analog pin 0
int raw = 0;           // variable to store the raw input value

void setup() {
  Serial.begin(9600);             // Setup serial
  digitalWrite(13, LOW);         // init off
}

void loop() {
  raw = 1024 - analogRead(analogPin);    // Reads the Input PIN
  Serial.print("Value: ");    
  Serial.println(raw);    
  if (raw < 400) {
    Serial.println("no signal");
    delay(200);
  } else {
    Serial.println("WE GET SIGNAL");
    lightUp();
  }    
} 

void lightUp() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(1000);                  // wait for a second
  digitalWrite(ledPin, LOW);   // set the LED off
}

