#include <ShiftRegLCD123.h>
#include <JeeLib.h>

const byte dataPin  = 10;    // SR Data from Arduino pin 10
const byte clockPin = 11;    // SR Clock from Arduino pin 11

// Instantiate a LCD object
ShiftRegLCD123 srlcd(dataPin, clockPin, SRLCD123);

DHTxx dht(5); 

void setup() {  
  // Initialize the LCD and set display size
  // LCD size 20 columns x 2 lines, defaults to small (normal) font
  srlcd.begin(16,1);
  
  // Turn on backlight (if used)
  srlcd.backlightOn();
  
  Serial.begin(57600);
  Serial.println("DHTxx Temperature Sensor with LCD");
  
  // can we use PWM to control constrast?
  pinMode(9, OUTPUT);
  digitalWrite(9, 0);
}

int t, h;
  
void loop() {
  if (dht.reading(t, h)) {
    double temp = t / 10.0;
    double humid = h / 10.0;
    
    Serial.print("temperature = ");
    Serial.println(temp);
    Serial.print("humidity = ");
    Serial.println(humid);
    Serial.println();
    
    srlcd.clear();
//    srlcd.setCursor(0, 0);
    srlcd.print("T ");
    srlcd.print(temp);
    srlcd.print("C");
    delay(1000);
    srlcd.clear();
//    srlcd.setCursor(0, 0);
    srlcd.print("H ");
    srlcd.print(humid);
    srlcd.print("%");
    delay(1000);
  }

}

