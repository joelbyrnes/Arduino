// receive whatever is being sent to broadcast or this node, show it

#include <Ports.h>
#include <RF12.h>
#include <RF12sio.h>

RF12 RF12;

void setup() {
    Serial.begin(57600);
    Serial.println("\n[rf12stream] - receive hello world");
    rf12_config();
}

void loop() {
    char value[RF12_MAXDATA];
  
    if (rf12_recvDone() && rf12_crc == 0) {
        Serial.print("Got \"");
        
        for (byte i = 0; i < rf12_len; ++i)
            Serial.print(rf12_data[i]);
        Serial.println("\"");
    }
}
