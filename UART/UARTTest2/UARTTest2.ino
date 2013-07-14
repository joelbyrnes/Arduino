/*
 * SC16IS750 UART-SPI bridge Example
 * http://www.sparkfun.com
 
 found at: http://www.geocities.jp/dotallcafe/arduino/SC16IS750_i2c.txt
 
 */
#include <Wire.h>
#include <SC16IS.h>

// Ana 5 ----- 2pin scl (pull-up 2.2k ohm)
// Ana 4 ----- 3pin sda (pull-up 2.2k ohm)

SC16IS uart;

int baudRate = 4800;

void setup() {
  //delay(500);
  Serial.begin(57600);
  Serial.println("\n\rSC16IS750 I2C Terminal Routine");
  
  generate38khz();
}

int connectUart() {
  // Test SPI communication
  if(uart.begin(0x90, baudRate))  {
    Serial.println("Bridge initialized successfully!"); 
    //sendData((byte*) "ahoy there!\n\r");
    return false;
  } else {
    Serial.println("Could not initialise I2C device."); 
    return true;
  }
}

// the FIFO buffer is 64 bytes so no use having a larger buffer than that 
// - although maybe for sends... 
#define SC16IS_BUFSIZE 64
byte inBuf[SC16IS_BUFSIZE];
byte adr = 0x90 >> 1;
#define THR        0x00 << 3
const byte thr = THR;

void sendBytes(byte* bytes, int n) {
//  Serial.print("sending: ");
//  Serial.println((char *) bytes);
  
  // TODO this should maybe check the size of the TX FIFO
  Wire.beginTransmission(adr);
  Wire.write(thr);
  for (int i = 0; i < n; i++) {
    Wire.write(bytes[i]);
  }
  Wire.endTransmission();
}

byte* readBytes(uint8_t n) {
  // will not attempt to get more bytes at a time than the buffer can hold
  if (n > SC16IS_BUFSIZE) 
    n = SC16IS_BUFSIZE;

  int get = requestBytes(n);

  int i = 0;
  while (i < get && Wire.available()) {
    inBuf[i++] = Wire.read();
  }

  return inBuf;
}

int requestBytes(uint8_t n) {
  Wire.beginTransmission(adr);
  Wire.write(thr);
  Wire.endTransmission();
  Wire.requestFrom(adr, n);
  return Wire.available();
}

long lastSend = 0;

void loop() {
  if (!uart.connected()) {
    if (!connectUart()) {
      delay(500);
      return;
    }
    // clear out the buffers
    readBytes(uart.available());
  }
  
  char polling = 0;
  
  // Poll for new data in SC16IS750 Recieve buffer 
  /*
  if(uart.available()) { 
    polling = 1;
    while(polling) {
      if(uart.available()) {
        incoming_data = uart.read();
        Serial.print(incoming_data);
      } else { polling = 0; }
    }
  }
  */
  
  int avail = uart.available();
  if (avail > 0) {
    char* data = (char*) readBytes(avail);
    data[avail] = NULL; // finalise string
    Serial.print(data);
  }

  // Otherwise, send chars from terminal to SC16IS750
  else if(Serial.available()) {
    // TODO read all bytes already in buffer before sending. arduino holds 128 bytes. 
    int avail = Serial.available();
    byte* bytes = readBytesFromSerial(avail);
//    Serial.print("checksum: ");
//    Serial.println(getCheckSum((char *) bytes), HEX);
    uart.writeBytes(bytes, avail);
    
    //incoming_data = Serial.read();
    //uart.write(incoming_data); 
  } //else sendBz();
}

void sendBz() {
  if (millis() - lastSend > 500) {
    uart.write("Bz", 2);
    lastSend = millis();
    Serial.println("sent Bz.");
  }
}

// this is 26 * 3 + 1 = 69 bytes so it wraps the 64 byte buffer and should be easy to see bits missing. 
// Aa.Bb.Cc.Dd.Ee.Ff.Gg.Hh.Ii.Jj.Kk.Ll.Mm.Nn.Oo.Pp.Qq.Rr.Ss.Tt.Uu.Vv.Ww.Xx.Yy.Zz.\n
// determined max is Aa.Bb.Cc.Dd.Ee.Ff.Gg.Hh.Ii.Jj.Kk.Ll.Mm.Nn.Oo.Pp.Qq.Rr.Ss.Tt.Uu.Vv.Ww.X\n = 72 bytes

#define BUFSIZE 128

byte serialBuffer[BUFSIZE];

byte* readBytesFromSerial(int n) {
  for (int i=0; i < n && i < BUFSIZE; i++) {
    serialBuffer[i] = Serial.read();
  }
  return serialBuffer;
}

char input[BUFSIZE];
int n;

char* readCharsFromSerial() {
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      Serial.println("overflow! discarding command. ");
      n = 0;
      break;
    }
    char in = Serial.read();
    if (in != '\n') {
      input[n++] = in;
    } else {
      input[n] = '\0'; // end string
      n = 0;
      return input; 
    }
  }
  // no newline reached and no more input for now; return null to indicate nothing to do.
  return NULL;
}

