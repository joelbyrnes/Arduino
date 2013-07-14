// Streaming layer demo, transfers different types of data in both directions
// 2009-05-07 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: rf12stream.pde 4727 2009-12-08 21:39:49Z jcw $

#include <Ports.h>
#include <RF12.h>
#include <RF12sio.h>

#define NODE_A 1
#define NODE_B 2
#define NODE_Z 26

MilliTimer testTimer;
RF12 RF12;
int count = 0;


void setup() {
  Serial.begin(57600);
  Serial.println("\n[rf12stream] - hello world test");
  rf12_config();
}

void loop() {
  char value[RF12_MAXDATA];

  String message = "echo hello world " + String(count++);
  message.toCharArray(value, message.length() + 1);
  
  recv();
  
  mustSendMsg(0, value, message.length() + 1);
}

void recv() {
  if (rf12_recvDone() && rf12_crc == 0) {
    Serial.print("Got \"");
    
    for (byte i = 0; i < rf12_len; ++i)
        Serial.print(rf12_data[i]);
    Serial.println("\"");
  } else {
    Serial.println("no recv");
  }
}

byte sendIfYouCan(byte dest, char *msg, byte sendLen) {
  if (rf12_canSend()) {
    sendMsg(dest, msg, sendLen);
    return 0;
  } else {
    Serial.println("can't send :(");
    return 1;
  }

  // it seems that a 5ms delay is the minimum needed to get canSend, 
  // after sending 18 bytes, on an unused channel.
  //delay(6);
}

void mustSendMsg(byte dest, char *msg, byte sendLen) {
  recv();
  while (!rf12_canSend()) {
    Serial.println("can't send :( waiting...");
    delay(2);
    recv();
  }
  
  sendMsg(dest, msg, sendLen);
}

// must have called canSend before this
void sendMsg(byte dest, char *msg, byte sendLen) {
  Serial.print(" -> ");
  Serial.print("node ");
  Serial.print((int) dest);  
  Serial.print(", ");
  Serial.print((int) sendLen);
  Serial.print("b: '");
  Serial.print(msg);
  Serial.println("'");
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
}

void sendAck(byte dest) {
  if (rf12_canSend()) {
    Serial.print(" -> ");
    Serial.println(" ACK");
    byte header = RF12_HDR_ACK;
    if (dest)
      header |= RF12_HDR_DST | dest;
    rf12_sendStart(header);
  }
}

