/*
 * SC16IS750 UART-SPI bridge Example
 * http://www.sparkfun.com
 
 found at: http://www.geocities.jp/dotallcafe/arduino/SC16IS750_i2c.txt
 
 */

#include <Wire.h>
// Ana 5 ----- 2pin scl (pull-up 2.2k ohm)
// Ana 4 ----- 3pin sda (pull-up 2.2k ohm)
// SC16IS750 Register Definitions
#define THR        0x00 << 3
#define RHR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TXFIFO     0x08 << 3
#define RXFIFO     0x09 << 3
#define DLAB       0x80 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

#define DLL        0x00 << 3
#define DLH        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3  
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3

// Communication flags and variables
int i2cadr = (0x90 >> 1);
char incoming_data = 0; 

byte thr = THR;

int connected = 0;

int baudRate = 4800;

void setup() {
  Wire.begin();
  Serial.begin(baudRate);
  Serial.println("\n\rSC16IS750 I2C Terminal Routine");
  
  generate38khz();
  
  while (!connected) {
    if (!connectUart(baudRate)) {
      delay(500);
    }
  }
}

void generate38khz() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Clear Timer on Compare Match (CTC) Mode
  bitWrite(TCCR1A, WGM10, 0);
  bitWrite(TCCR1A, WGM11, 0);
  bitWrite(TCCR1B, WGM12, 1);
  bitWrite(TCCR1B, WGM13, 0);

  // Toggle OC1A and OC1B on Compare Match.
  bitWrite(TCCR1A, COM1A0, 1);
  bitWrite(TCCR1A, COM1A1, 0);
  bitWrite(TCCR1A, COM1B0, 1);
  bitWrite(TCCR1A, COM1B1, 0);

  // No prescaling
  bitWrite(TCCR1B, CS10, 1);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS12, 0);

  OCR1A = 210;
  OCR1B = 210;

  Serial.println("Generating 38kHz signal on pins 9 & 10");
}

int connectUart(long baud) {
  // Test SPI communication
  if(Uart_Init(baud))  {
    Serial.println("Bridge initialized successfully!"); 
    connected = 1;
    //sendData((byte*) "ahoy there!\n\r");
  }
  else 
    Serial.println("Could not initialise I2C device."); 
  return connected;
}
/*
void sendData(byte* str) {
  Serial.print("sending: ");
  Serial.println((char *) str);
  
  Wire.beginTransmission(i2cadr);
  Wire.send(THR);
  Wire.send(str);
  Wire.endTransmission();
}
*/
byte inBuf[100];

byte* readData() {
  Wire.beginTransmission(i2cadr);
  Wire.write(thr);
  Wire.endTransmission();
  Wire.requestFrom(i2cadr, 1);
  int i = 0;
  while (Wire.available() && i < 100) {
    inBuf[i++] = Wire.read();
  }
  return inBuf;
}

long lastSend = 0;

void loop() {
  // Poll for new data in SC16IS750 Recieve buffer 
  if(I2C_ReadByte(LSR) & 0x01) { 
    while((I2C_ReadByte(LSR) & 0x01)) {
      incoming_data = I2C_ReadByte(RHR);
      Serial.print(incoming_data);
    }
  }
  // Otherwise, send chars from terminal to SC16IS750
  else if(Serial.available()) {
    incoming_data = Serial.read();
    I2C_WriteByte(THR, incoming_data); // Transmit command
  } else {
    sendAz();
  }
}

void sendAz() {
  if (millis() - lastSend > 500) {
    I2C_WriteByte(THR, 'A');
    I2C_WriteByte(THR, 'z');
    lastSend = millis();
    Serial.println("sent Az.");
  }
}

void setBaud(long baud) {
  long divisor = 14745600 / (baud * 16);
  uint8_t dll = divisor & 0x00FF;
  // get top byte and shift down 
  uint8_t dlh = (divisor & 0xFF00) >> 8;

  I2C_WriteByte(LCR, 0x80);  // 0x80 to program baudrate

  // 0x60 = 9600, 0x30=19.2K, 0x08 =115.2K with Xtal = X1=14.7456 MHz

  I2C_WriteByte(DLL, dll);
  I2C_WriteByte(DLH, dlh); 
}

// Initialize SC16IS750
char Uart_Init(long baud) {
//  I2C_WriteByte(MCR,0x40);  // enable IrDA
  
  setBaud(baud);
 
  I2C_WriteByte(LCR, 0xBF); // access EFR register
  I2C_WriteByte(EFR, 0x10); // enable enhanced registers, no flow control
  I2C_WriteByte(LCR, 0x03); // 8 data bit, 1 stop bit, no parity
  I2C_WriteByte(FCR, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
  I2C_WriteByte(FCR, 0x01); // enable FIFO mode

  // Perform read/write test to check if UART is working
  I2C_WriteByte(SPR,'H');
  char data = I2C_ReadByte(SPR);

  if(data == 'H')  return true; 
  else             return false; 
}
byte I2C_ReadByte(byte reg) {
  Wire.beginTransmission(i2cadr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cadr, 1);  
  byte data = Wire.read();  
  return data;
}
void I2C_WriteByte(byte reg, byte data) {
  Wire.beginTransmission(i2cadr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

