// receive whatever is being sent to broadcast or this node, show it

#include <Ports.h>
#include <RF12.h>
#include <RF12sio.h>

RF12 RF12;
char input[RF12_MAXDATA];

void setup() {
    Serial.begin(57600);
    Serial.println("\n[rf12listener] - print received packets");
    rf12_config();
}

void loop() {
    char value[RF12_MAXDATA];
  
    if (rf12_recvDone() && rf12_crc == 0) {
        //Serial.print("Got \"");

        // copy packet into buffer before it disappears
        for (byte i = 0; i < rf12_len; ++i) {
          if (rf12_data[i] != '\n') {
            input[i] = rf12_data[i];
          } else {
            input[i] = '\0'; // end string
          }
        }
        Serial.println(input);
        
        /*
        for (byte i = 0; i < rf12_len; ++i)
            Serial.write(rf12_data[i]);
        */    
            
        //Serial.println("\"");
    }
}
