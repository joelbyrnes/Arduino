#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CSN, radio pins 3 & 4)

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

boolean ready = false;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(void)
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  printf_begin();
  printf("\n\r RF24Sender \n\r");

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  radio.startListening();
  radio.printDetails();
  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

boolean radioConnected() {
  printf("PayloadSize = %i \n", radio.getPayloadSize()); // appears to be stored in library
  printf("CRCLength = %i \n", radio.getCRCLength());
  printf("DynamicPayloadSize = %i \n", radio.getDynamicPayloadSize()); // sometimes starts at 0
  printf("isPVariant = %i \n", radio.isPVariant());
  printf("PALevel = %i \n", radio.getPALevel());
  
  // TODO how to read back current address? 
  // TODO write something and see if it appears to send?

  // try to read values from the radio to determine if it is even connected, assuming these are all configured > 0
  if (radio.getCRCLength() && radio.isPVariant() && radio.getPALevel()) {
    return true;
  } 
  return false;
}

void loop(void) {
  if (!ready) {
    if (radioConnected()) {
      ready = true;
      printf("Radio ready.\n");
    } else {
      // probably any one of CSN (normally 10), MOSI, MISO or SCK (11, 12, 13) are not connected correctly 
      printf("Could not read from radio.\n\n");
      delay(500);
      return;
    }
  }

  // print the string when a newline arrives:
  if (stringComplete) {
    sendMsg(inputString); 
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

unsigned char send_buf[200];
unsigned char read_buf[200];

void sendMsg(String msg) {
  // First, stop listening so we can talk.
  radio.stopListening();

  msg.getBytes(send_buf, min(msg.length(), 200));
  send_buf[min(msg.length(), 200-1)] = 0;

  printf("Sending %s... ", send_buf);
  int last_read = radio.write( send_buf, strlen((const char*) send_buf));
  
  if (last_read)
    printf("ok...");
  else
    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout ) {
    printf("Failed, response timed out.\n\r");
  } else {
    // Grab the response, compare, and send to debugging spew
    radio.read( read_buf, sizeof(read_buf) );

    // Spew it
    printf("Received: '%s'\n", read_buf);
  }

}

