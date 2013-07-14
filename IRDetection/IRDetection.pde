#include <NewSoftSerial.h>

void setup() {
  Serial.begin(9600);
  Serial.println("test begins");
}
int count = 0;
int on = 0;

void loop() {
  //Serial.print(digitalRead(52));
    
  if (digitalRead(52) == HIGH) {
    //if (on == 0) {
      Serial.print("ARGH! ");
      Serial.println(count++);
      on = 1;
    //}
  } else {
    on = 0;
  }
  
}


