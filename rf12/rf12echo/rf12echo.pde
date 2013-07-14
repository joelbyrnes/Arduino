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

char value[RF12_MAXDATA];

void setup() {
  Serial.begin(57600);
  Serial.println("\n[rf12stream] - echo");
  rf12_config();
  value[0] = NULL;
}

void loop() {

  if (rf12_recvDone() && rf12_crc == 0) {
    for (byte i = 0; i < rf12_len; ++i) {
      value[i] = rf12_data[i];
    }
    // TODO need protection for max message size here
    // end the string
    value[rf12_len] = NULL;
  }

  if (value[0] != NULL) {
    Serial.print(" <- node ");
    Serial.print((int) RF12_HDR_MASK & rf12_hdr);
    Serial.print(", ");
    Serial.print(strlen(value));
    Serial.print("b : '");
    Serial.print(value);
    Serial.println("', replying");
    
    char reply[RF12_MAXDATA];
    
    String replyStr = "echo: " + String(value);
    replyStr.toCharArray(reply, replyStr.length());

    mustSendMsg(0, reply, strlen(reply));
    
    value[0] = NULL;
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
}

void mustSendMsg(byte dest, char *msg, byte sendLen) {
  recvAndThrowAway();
  while (!rf12_canSend()) {
    Serial.println("can't send :( waiting...");
    //delay(2);
    recvAndThrowAway();
  }
  
  sendMsg(dest, msg, sendLen);
}

void recvAndThrowAway() {
  if (rf12_recvDone()) {
    Serial.println("OH GOD I RECEIVED AND IM IGNORING IT");
  }
}

// must have called canSend before this
void sendMsg(byte dest, char *msg, byte sendLen) {
  Serial.print(" -> ");
  Serial.print("node ");
  Serial.print((int) dest);  
  Serial.print(", ");
  Serial.print((int) sendLen);
  Serial.print("b : '");
  Serial.print(msg);
  Serial.println("'");
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
}

