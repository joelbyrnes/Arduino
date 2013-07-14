
#include <NewSoftSerial.h>

NewSoftSerial mySerial(4, 5);

void setup()  
{
  Serial.begin(57600);
  Serial.println("Goodnight moon!");

  // set the data rate for the NewSoftSerial port
  mySerial.begin(19200);
  mySerial.println("Hello, world?");
}

void loop() {
  echoOther();
}  

void newSerialEcho() {
  if (mySerial.available()) {
      mySerial.print((char)mySerial.read());
  }
}
  
void oldSerialEcho() {
  if (Serial.available()) {
      Serial.print((char)Serial.read());
  }
}
  
void echoOther() {
  if (mySerial.available()) {
      Serial.print((char)mySerial.read());
  }
  if (Serial.available()) {
      mySerial.print((char)Serial.read());
  }
}

