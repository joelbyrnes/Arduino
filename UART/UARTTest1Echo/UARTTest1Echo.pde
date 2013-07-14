/*
 * SC16IS750 UART-SPI bridge Example
 * http://www.sparkfun.com
 
 found at: http://www.geocities.jp/dotallcafe/arduino/SC16IS750_i2c.txt
 
 */
 
#define BUFSIZE 100

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
#define DLM        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3  
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3

// Communication flags and variables
int i2cadr = (0x90 >> 1);
char incoming_data = 0; 
char polling = 0;

int connected = 0;

void setup() {
  //delay(500);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\n\rSC16IS750 I2C Terminal Routine");
}

int connectUart() {
  // Test SPI communication
  if(Uart_Init())  {
    Serial.println("Bridge initialized successfully!"); 
    connected = 1;
    sendString("ahoy there!\n\r");
  }
  else 
    Serial.println("Could not initialise I2C device."); 
  return connected;
}

void sendString(char* str) {
  sendData((byte*) str);
}

void sendData(byte* str) {
  Serial.print("sending: ");
  Serial.println((char *) str);
  
  Wire.beginTransmission(i2cadr);
  Wire.send(THR);
  // TODO does this work or do I need to send THR for each char?
  for (int i = 0; str[i] != NULL; i++) {
    Wire.send(str[i]);
  }
  Wire.endTransmission();
}

byte inBuf[BUFSIZE];

byte* readData() {
  Wire.beginTransmission(i2cadr);
  Wire.send(THR);
  Wire.endTransmission();
  // what happens if less bytes are available than requested? will it return less or wait till they are available, or neither?
  Wire.requestFrom(i2cadr, BUFSIZE);
  int i = 0;
  while (Wire.available() && i < BUFSIZE) {
    inBuf[i++] = Wire.receive();
  }
  return inBuf;
}
  
char input[BUFSIZE];
int n;

void loop() {
  if (!connected) {
    if (!connectUart()) {
      delay(500);
      return;
    }
  }

  // Poll for new data in SC16IS750 Recieve buffer 
  if(I2C_ReadByte(LSR) & 0x01) { 
    // TODO does this work?
    /*
    byte* data = readData();
    sendString("echo: ");
    sendData(data);
    Serial.print("echoing: ");
    Serial.println((char*) data);
    */
    
    polling = 1;
    while(polling) {
      if((I2C_ReadByte(LSR) & 0x01)) {
        if (n >= BUFSIZE) {
          Serial.println("overflow! discarding command. ");
          n = 0;
        }

        incoming_data = I2C_ReadByte(RHR);
        
        if (incoming_data != '\n') {
          input[n++] = incoming_data;
        } else {
          input[n] = '\0'; // end string
          processData(input);    
          n = 0;
        }
        
        Serial.print(incoming_data);
      } else { polling = 0; }
    }
  }
  // Otherwise, send chars from terminal to SC16IS750
  else if(Serial.available()) {
    incoming_data = Serial.read();
    I2C_WriteByte(THR, incoming_data); // Transmit command
  } 
}

void processData(char data[]) {
  char *tok;
  char *value;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "echo") == 0) {
      Serial.print("reply: ");
      if (value != NULL) {
        Serial.println(value);
      }
    } else {
      Serial.println("what?");
    }
    tok = strtok(NULL, " \t");
  }
}

// Initialize SC16IS750
char Uart_Init(void) {
  I2C_WriteByte(LCR,0x80);  // 0x80 to program baudrate
  I2C_WriteByte(DLL,0x08);  // 0x60 = 9600, 0x30=19.2K, 0x08 =115.2K with Xtal = X1=14.7456 MHz
  I2C_WriteByte(DLM,0x00);  // divisor = 0x0008 for 115200 bps -- um no it's not? just DLL of 0x08. changing this screws it. 

  I2C_WriteByte(LCR, 0xBF); // access EFR register
  I2C_WriteByte(EFR, 0x10); // enable enhanced registers
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
  Wire.send(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cadr, 1);  
  byte data = Wire.receive();  
  return data;
}
void I2C_WriteByte(byte reg, byte data) {
  Wire.beginTransmission(i2cadr);
  Wire.send(reg);
  Wire.send(data);
  Wire.endTransmission();
}

