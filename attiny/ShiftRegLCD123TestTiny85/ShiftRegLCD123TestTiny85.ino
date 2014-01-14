#include <ShiftRegLCD123.h>

const byte dataPin  = 3;    // attiny85 analog input 3, pin 3, physical pin 2
const byte clockPin = 4;    // attiny85 analog input 2, pin 4, physical pin 4

// Instantiate a LCD object
ShiftRegLCD123 srlcd(dataPin, clockPin, SRLCD123);

void setup()
{
  // Initialize the LCD and set display size
  // LCD size 20 columns x 2 lines, defaults to small (normal) font
  srlcd.begin(16,1);
  
  // Turn on backlight (if used)
  srlcd.backlightOn();
  
  // Print a message to the LCD.
  srlcd.print("HELLO, WORLD!");
}

void loop()
{
  srlcd.clear();
  srlcd.print("-HELLO, WORLD!");
}
