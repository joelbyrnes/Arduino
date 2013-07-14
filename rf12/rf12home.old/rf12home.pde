
#include <Ports.h>
#include <RF12.h>

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
//RF12 RF12;
byte needToSend = 0;
byte thisId = 0;

void setup() {
    Serial.begin(57600);
    Serial.println("\n[rf12] - home base");
    thisId = rf12_config();
}

char data[RF12_MAXDATA];
int dataLength = 0;

void loop() {
    // read from serial input into buffer
    if (Serial.available()) {
      while (Serial.available()) {
        // TODO need input size protection in here - max dataLength is 61 apparently?
        data[dataLength++] = Serial.read();
      }
      //Serial.print("read some data from serial: '");
      //Serial.print(data);
      //Serial.print("', length = ");
      //Serial.println(dataLength);
      
      // if a whole message, prepare to send
      if (data[dataLength - 1] == '\n') {
        //Serial.println("got msg with newline, will send");
        needToSend = 1;
        data[dataLength] = '\0';
      }      
    }

    // print a received packet
    if (rf12_recvDone() && rf12_crc == 0) {
      //Serial.print("<");
      //Serial.print((int) RF12_HDR_MASK & rf12_hdr);
      //Serial.print("> ");    
      for (byte i = 0; i < rf12_len; ++i)
          Serial.print(rf12_data[i]);
      //Serial.println();
    }
    
    if (needToSend && rf12_canSend()) {
      needToSend = 0;
      payload.print(data);
      rf12_sendStart(0, payload.buffer(), dataLength);
      payload.reset();
      clearString(data, RF12_MAXDATA);
      dataLength = 0;
    }
    
    //sendTest();
}

void clearString(char *string, int length) {
  for (int i = 0; i < length; i++) {
    string[i] = NULL;
  }
}

void sendTest() {
    // we intend to send once a second
    if (sendTimer.poll(1000)) {
        needToSend = 1;
    }

    // can only send when the RF12 driver allows us to
    if (needToSend && rf12_canSend()) {
        needToSend = 0;
        // fill the packet buffer with text to send
        payload.print("myId = ");
        payload.print(thisId, DEC);
        payload.print(", millis = ");
        payload.print(millis());
        // send out the packet
        //sendPayloadX(0, payload);
        rf12_sendStart(0, payload.buffer(), payload.length());
        payload.reset();
    }
}
/* wtf?
void sendPayloadX(byte header, PacketBuffer payload) {
  rf12_sendStart(header, payload.buffer(), payload.length());
}
*/
