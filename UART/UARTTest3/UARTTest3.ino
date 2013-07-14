#include <Wire.h>
#include <SC16IS.h>
 
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

void sendChecksummed(char* str) {
  targetPayload.print(str);
  
  Serial.print("message: '");
  Serial.print((char*) targetPayload.buffer());
  Serial.println("'");
  
  targetPayload.write(generateChecksum((byte*) targetPayload.buffer(), 2));
  targetPayload.print("\n");
  
//  const byte* bytes = targetPayload.buffer();
 
  targetPayload.write(NULL);
  
  Serial.print("sending: '");
  Serial.print((char*) targetPayload.buffer());
  Serial.println("'");
  
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
    beacon("A1");
    //Serial.println("sent Cz.");
    lastSend = millis();
  }
}

char letter = 'A'; 

void sendTest() {
  // it should take about 12.5ms to send 3 chars at 2400 baud, so 20ms is fine. 
  if (millis() - lastSend > 20) {
    targetPayload.print(letter);
    sendChecksummed("z");
    lastSend = millis();
    Serial.print("sent ");
    Serial.print(letter);
    Serial.println("z.");
    letter++;
    if (letter > 'A' + 25) letter = 'A';
  }
}

// send message over uart, keep the buffer full so it sends constantly. 
void beacon(char* beaconMsg) {
  // every 60ms is plenty to keep the 64 byte buffer full without doing Wire calls every loop. 
  if (millis() - lastBeacon < 60) return;
  int len = strlen(beaconMsg);
  int msgLen = len + 2;
  int avail = uart.txBufferAvailable();
  //aprintf("tx buffer has %d b avail", avail);
  if (avail < msgLen) return;

  //aprintf("sending beacon '%s'", msg);

  byte cs = generateChecksum((byte*) beaconMsg, len);
  
  int c = 0;
  while (avail > msgLen) {
    beaconOutput.print(beaconMsg);
    beaconOutput.write(cs);
    beaconOutput.print("\n");
    avail -= msgLen;
    //if (c++ == 0) aprintf("buffer: %s", beaconOutput.buffer());
  }
  beaconOutput.write(NULL);
  aprintf("buffer: %s", beaconOutput.buffer());
  uart.print((char *) beaconOutput.buffer());
  beaconOutput.reset();
  lastBeacon = millis();
}

void sendPayload() {
  targetPayload.write(NULL);
  uart.write((char *) targetPayload.buffer());
  targetPayload.reset();
}

