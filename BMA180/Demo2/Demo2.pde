#include <Wire.h>
 
// TWI (I2C) sketch to communicate with the LIS3LV02DQ accelerometer
// Using the Wire library (created by Nicholas Zambetti)
// http://wiring.org.co/reference/libraries/Wire/index.html
// On the Arduino board, Analog In 4 is SDA, Analog In 5 is SCL
// These correspond to pin 27 (PC4/ADC4/SDA) and pin 28 (PC5/ADC5/SCL) on the Atmega8
// The Wire class handles the TWI transactions, abstracting the nitty-gritty to make
// prototyping easy.
 
void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  Serial.begin(9600);
 
  CLKPR = (1<<CLKPCE);
  CLKPR = 0;
 
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.beginTransmission(0x1D);
  Wire.send(0x20); // CTRL_REG1 (20h)
  Wire.send(0x87); // Device on, 40hz, normal mode, all axis's enabled
  Wire.endTransmission();
 
}
 
void loop()
{
 
  byte z_val_l, z_val_h, x_val_l, x_val_h, y_val_l, y_val_h;
  int z_val, x_val, y_val;
  //Serial.println("hello?");
  //byte in_byte;
  // transmit to device with address 0x1D
  // according to the LIS3L* datasheet, the i2c address of is fixed
  // at the factory at 0011101b (0x1D)
  Wire.beginTransmission(0x1D);
  // send the sub address for the register we want to read
  // this is for the OUTZ_H register
  // n.b. supposedly masking the register address with 0x80,
  // you can do multiple reads, with the register address auto-incremented
  Wire.send(0x28);
  // stop transmitting
  Wire.endTransmission();
 // Now do a transfer reading one byte from the LIS3L*
 // This data will be the contents of register 0x28
 Wire.requestFrom(0x1D, 1);
  while(Wire.available())
 {
   x_val_l = Wire.receive();
 }
 Wire.beginTransmission(0x1D); Wire.send(0x29); Wire.endTransmission();
 Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
    x_val_h = Wire.receive();
 }
 x_val = x_val_h;
 x_val <<= 8;
 x_val += x_val_l;
 
 // Y Axis
 Wire.beginTransmission(0x1D); Wire.send(0x2A); Wire.endTransmission();
 Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
    y_val_l = Wire.receive();
 }
 Wire.beginTransmission(0x1D); Wire.send(0x2B); Wire.endTransmission();
 Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
    y_val_h = Wire.receive();
 }
 
 y_val = y_val_h;
 y_val <<= 8;
 y_val += y_val_l; 
 
// Z Axis
 Wire.beginTransmission(0x1D); Wire.send(0x2C); Wire.endTransmission();
 Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
    z_val_l = Wire.receive();
 }
 Wire.beginTransmission(0x1D); Wire.send(0x2D); Wire.endTransmission();
 Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
    z_val_h = Wire.receive();
 }
 
 z_val = z_val_h;
 z_val <<= 8;
 z_val += z_val_l;
  // Set up to read the next register
  // Supposedly, if you set the most significant bit of
  // the register address to 1, you can do a multiple read,
  // with the register address auto-incrementing. This way, you
  // don't have to do another write — specifying the next register —
  // before reading the next register value. This would be very good, but
  // with a cursory attempt, I have yet to make this work.
/*
    Wire.beginTransmission(0x1D); // transmit to device #4
  Wire.send(0x2C);        // read outx_h
  // stop transmitting
  Wire.endTransmission();
  Wire.requestFrom(0x1D, 1);
 while(Wire.available())
 {
   z_val_l = Wire.receive();
 }
 
 z_val = z_val_h;
 z_val <<= 8;
 z_val += z_val_l;
 
  // z axis acceleration, all set.
  // Perfectly still and with the z-axis parallel to the ground, it should be about 1024-ish
  // (I find in practice it is around 1019.)
  // When the z-axis is orthogonal to the ground, it should be zero.
  // 1024 is +1g acceleration, or normal gravity
  // 2048 is +2g
  // 0 is 0g
  Serial.println(z_val, DEC);
*/
Serial.print(x_val, HEX); Serial.print(" ");Serial.print(y_val, HEX); Serial.print(" "); Serial.println(z_val, HEX);
delay(100);
}

