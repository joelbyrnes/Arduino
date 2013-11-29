#include <ShiftRegLCD123.h>

const byte dataPin  = 10;    // SR Data from Arduino pin 10
const byte clockPin = 11;    // SR Clock from Arduino pin 11

// Instantiate a LCD object
ShiftRegLCD123 srlcd(dataPin, clockPin, SRLCD123);

void setup()
{
  // Initialize the LCD and set display size
  // LCD size 20 columns x 2 lines, defaults to small (normal) font
  srlcd.begin(20,2);
  
  // Turn on backlight (if used)
  srlcd.backlightOn();
  
  // Print a message to the LCD.
  srlcd.print("HELLO, WORLD!");
}

void loop()
{
}
