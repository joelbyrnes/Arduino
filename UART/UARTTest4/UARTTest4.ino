#include <Wire.h>
#include <SC16IS.h>
 
#include <stdio.h>
char _str[128]; // 128 chars max!  increase if required to avoid overflow
#define aprintf(...) sprintf(_str, __VA_ARGS__); Serial.println(_str)

class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; while (fill < sizeof buf) buf[fill++] = NULL; fill = 0; }

    size_t write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[80];
};

PacketBuffer targetPayload;   // sending buffer

SC16IS uart;

int baudRate = 2400;

#define BUFSIZE 128
char input[BUFSIZE];
int n;

void setup() {
  Serial.begin(57600);
  Serial.println("\n\rSC16IS750 I2C Terminal Routine");
  
  generate38khz();
}

int connectUart() {
  // Test SPI communication
  if(uart.begin(0x90, baudRate))  {
//    uart.enableIrDA(true);
    Serial.println("Bridge initialized successfully!"); 
    //sendData((byte*) "ahoy there!\n\r");
    return false;
  } else {
    Serial.println("Could not initialise I2C device."); 
    return true;
  }
}

void loop() {
  if (!uart.connected()) {
    if (!connectUart()) {
      delay(500);
      return;
    }
    // clear out the buffers?
    uart.readBytes(uart.available());
  }

  int avail = uart.available();
  if (avail > 0) {
    char* data = (char*) uart.readBytes(avail);
    data[avail] = NULL; // finalise string if it wasn't already
    Serial.print("recv: '");
    Serial.print(data);
    Serial.println("'");
  }

  // Otherwise, send chars from terminal to SC16IS750
  else if(Serial.available()) {
    char* chars = readLineFromSerial();
//    Serial.print("checksum: ");
//    Serial.println(getCheckSum((char *) bytes), HEX);
    if (chars != NULL) {
      Serial.print("read line from serial, len: ");
      Serial.println(strlen(chars));
      uart.write(chars, strlen(chars));
    } else {
      Serial.println("no line from serial");
    }

  } else sendTest();
}

char* readLineFromSerial() {
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      Serial.println("overflow! discarding command. ");
      n = 0;
      break;
    }
    char in = Serial.read();
    input[n++] = in;
    if (in == '\n') {
      input[n] = '\0'; // end string
      n = 0;
      return input; 
    }
  }
  // no newline reached and no more input for now; return null to indicate nothing to do.
  return NULL;
}

long lastSend = 0;

void sendChecksummed(byte* bytes) {
  targetPayload.print((char*) bytes);
  
  Serial.print("message: '");
  Serial.print((char*) targetPayload.buffer());
  Serial.println("'");
  
  targetPayload.write(generateChecksum((byte*) targetPayload.buffer(), 2));
  targetPayload.print("\n");
  
//  const byte* bytes = targetPayload.buffer();
 
  targetPayload.write(NULL);
  
//  Serial.print("sending: '");
//  Serial.print((char*) targetPayload.buffer());
//  Serial.println("'");
  
  uart.write((char *) targetPayload.buffer());
  targetPayload.reset();
    
//  Serial.print("with cs is: ");
//  Serial.println((char*) bytes);
//  
//  if (validateChecksum((byte*) bytes, 3)) {
//    Serial.println("valid.");
//  } else {
//    Serial.println("invalid.");
//  }
//  Serial.print("message is: ");
//  Serial.println((char*) bytes);
//  
//  Serial.println();
}

void sendCz() {
  if (millis() - lastSend > 500) {
    //sendChecksummed("Cz");
    beacon();
    //Serial.println("sent Cz.");
    lastSend = millis();
  }
}

char letter = 'A'; 

void sendTest() {
  beaconAtoZ();
}

char* beaconMsg = "1A";

void beaconAtoZ() {
  // every 60ms is plenty to keep the 64 byte buffer full without doing Wire calls every loop. 
  if (millis() - lastSend < 60) return;
  
  int avail = uart.txBufferAvailable();
  aprintf("refilling buffer");
  
 
//  beaconMsg[1] = letter;
//  if (++letter > 'A' + 25) letter = 'A';

  while (avail > 4) {
    byte foo = beaconMsg[0];
    if (++foo > 254) foo = 1;
    beaconMsg[0] = foo;
 
    sendChecksummed((byte*) beaconMsg);
    avail -= 4;
    lastSend = millis();
  }
}

unsigned long lastBeacon = 0;

// send message over uart, keep the buffer full so it sends constantly. 
void beacon() {
  // every 60ms is plenty to keep the 64 byte buffer full without doing Wire calls every loop. 
  if (millis() - lastBeacon < 60) return;
  int len = strlen(beaconMsg);
  int msgLen = len + 2;
  int avail = uart.txBufferAvailable();
  //aprintf("tx buffer has %d b avail", avail);
  if (avail < msgLen) return;

  //aprintf("sending beacon '%s'", msg);
  aprintf("refilling buffer");

  byte cs = generateChecksum((byte*) beaconMsg, len);
  
  int c = 0;
  while (avail > msgLen) {
    targetPayload.print(beaconMsg);
    targetPayload.write(cs);
    targetPayload.print("\n");
    avail -= msgLen;
    //if (c++ == 0) aprintf("buffer: %s", beaconOutput.buffer());
  }
  targetPayload.write(NULL);
  aprintf("buffer: %s", targetPayload.buffer());
  uart.print((char *) targetPayload.buffer());
  targetPayload.reset();
  lastBeacon = millis();
}

void sendPayload() {
  targetPayload.write(NULL);
  uart.write((char *) targetPayload.buffer());
  targetPayload.reset();
}

