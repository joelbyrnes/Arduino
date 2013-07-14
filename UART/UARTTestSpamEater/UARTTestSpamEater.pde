#include <Wire.h>
#include <SC16IS.h>

SC16IS uart;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\rSC16IS750 I2C Terminal Routine");
  
  connectUart();
  while (!uart.connected()) {
    delay(500);
    connectUart();
  }
}

int connectUart() {
  if(uart.begin(0x90, 57600))  {
    Serial.println("Bridge initialized successfully!"); 
    return false;
  } else {
    Serial.println("Could not initialise I2C device."); 
    return true;
  }
}

// the FIFO buffer is 64 bytes so no use having a larger buffer than that 
// - although maybe for sends... 
#define SC16IS_BUFSIZE 100
byte inBuf[SC16IS_BUFSIZE];
byte adr = 0x90 >> 1;
#define THR        0x00 << 3

void sendBytes(byte* bytes, int n) {
  Serial.print("sending: ");
  Serial.println((char *) bytes);
  
  Wire.beginTransmission(adr);
  Wire.send(THR);
  for (int i = 0; i < n; i++) {
    Wire.send(bytes[i]);
  }
  Wire.endTransmission();
}

int sc16is_rcvd = 0;

byte* readBytes(int n) {
  // will not attempt to get more bytes at a time than the buffer can hold
  if (n > SC16IS_BUFSIZE) 
    n = SC16IS_BUFSIZE;
  Wire.beginTransmission(adr);
  Wire.send(THR);
  Wire.endTransmission();

  Wire.requestFrom((int) adr, n);
  int i = 0;
  while (i < n && Wire.available()) {
    inBuf[i++] = Wire.receive();
  }
  inBuf[i] = '\0';
  sc16is_rcvd = i;
 
  return inBuf;
}

int n;
int x;
void loop() {
  //printBytes();
  //testSize();
  //doBufThing();
  doBetter();
}

byte buf[80];
int bufpos;

void doBetter() {
  bufpos = 0;
  while (uart.available()) {
    Wire.beginTransmission(adr);
    Wire.send(THR);
    Wire.endTransmission();

    Wire.requestFrom((int) adr, 100);
    
    while (bufpos < 80 && Wire.available()) {
      buf[bufpos++] = Wire.receive();
    }

  }
  // buf is full or all input gotten

  if (bufpos > 0) {
    Serial.print("got bytes: ");
    Serial.println(bufpos);
  }

  buf[bufpos] = 0;
  Serial.println((char*) buf);
  
}



void printBytes() {
  while (uart.available()) {
    byte* data = readBytes(SC16IS_BUFSIZE);
//    if (data) {
      Serial.print((char*) data);
      data = 0;
//    }
    Serial.print('+');
  }
  //Serial.print('-');
}
  
void testSize() {
  n = 0;
  x = 0;
  while (uart.available()) {
    byte* data = readBytes(SC16IS_BUFSIZE);
    n++;
    x += sc16is_rcvd;
  }
  if (n > 0) {
    Serial.print("read data ");
    Serial.print(n);
    Serial.print(" times, size is ");
    Serial.println(x);
  }
}

#define BIGBUF_SIZE 200
byte bigBuf[BIGBUF_SIZE];
int bufOffset = 0;

void doBufThing() {
  
  //delay(200);
  while (uart.available() && bufOffset < BIGBUF_SIZE) {
    char* data = (char*) readBytes(SC16IS_BUFSIZE);
    //if (data[0] != '\0')
    
    // make sure bytes to copy is not going to overflow the buffer
    int bytesToCopy = sc16is_rcvd;
    if (bufOffset + bytesToCopy > BIGBUF_SIZE) {
      bytesToCopy = BIGBUF_SIZE - bufOffset;
    }
    
    //Serial.print("copying into bigBuf: ");
    //Serial.println(bytesToCopy);
    // copy inBuf into bigBuf at position bufOffset, the number of bytes copied
    memcpy(&bigBuf[bufOffset], inBuf, bytesToCopy);
    // advance bufPtr by the number of bytes copied
    bufOffset += bytesToCopy;
  }
  if (bufOffset > 0) {
    Serial.print("bigBuf is now ");
    Serial.print(bufOffset);
    Serial.println(" bytes");
  }

  bigBuf[bufOffset] = 0;
  Serial.print((char*) bigBuf);

  bufOffset = 0;
}







