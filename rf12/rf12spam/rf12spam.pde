// Streaming layer demo, transfers different types of data in both directions
// 2009-05-07 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: rf12stream.pde 4727 2009-12-08 21:39:49Z jcw $

#include <Ports.h>
#include <RF12.h>
//#include <RF12sio.h>

class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }

    virtual void write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[RF12_MAXDATA];
};

PacketBuffer payload;   // sending buffer
MilliTimer sendTimer;
int count = 0;
byte needToSend = 0;

void setup() {
    Serial.begin(57600);
    Serial.println("\n[rf12] - home base");
    rf12_config();
}

void loop() {
    // we intend to send once a second
    if (sendTimer.poll(50)) {
        needToSend = 1;
    }

  // print a received packet
    if (rf12_recvDone() && rf12_crc == 0) {
      //Serial.print("<");
      //Serial.print((int) RF12_HDR_MASK & rf12_hdr);
      //Serial.print("> ");    
      //for (byte i = 0; i < rf12_len; ++i)
          //Serial.print(rf12_data[i]);
      //Serial.println();
      
      Serial.print('R');
      
      if (count % 40 == 0) {
        Serial.println();
      }
    }
    
    spam();    
}

void spam() {
    // can only send when the RF12 driver allows us to
    if (needToSend && rf12_canSend()) {
      needToSend = 0;
      payload.print("echo hw");
      payload.print(count++);
      //Serial.print("sending: ");
      //Serial.println((char*) payload.buffer());
      Serial.print(".");
      rf12_sendStart(0, payload.buffer(), payload.length());
      payload.reset();
    }
}

