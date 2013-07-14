#include <Wire.h>

#define address 0x40

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  initBMA180();
  delay(2000);
}

void loop()
{

  readAccel();

  delay(10);
}

