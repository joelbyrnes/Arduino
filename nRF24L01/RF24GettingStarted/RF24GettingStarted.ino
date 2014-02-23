/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two 
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.  The ping node sends the current 
 * time to the pong node, which responds by sending the value back.  The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CSN, radio pins 3 & 4)

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The default role of the current running sketch
role_e role = role_pong_back;

boolean ready = false;

void setup(void)
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //
  
  if ( role == role_ping_out ) set_role_ping_out();
  else set_role_pong_back();

  radio.startListening();
  radio.printDetails();
}

// This simple sketch opens two pipes for these two nodes to communicate
// back and forth.
// Open 'our' pipe for writing
// Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

void set_role_ping_out() {
  // Become the transmitter (ping out)
  role = role_ping_out;
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
}

void set_role_pong_back() {
  // Become the receiver (pong back)
  role = role_pong_back;
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
}

boolean radioConnected() {
  printf("PayloadSize = %i \n", radio.getPayloadSize()); // appears to be stored in library
  printf("CRCLength = %i \n", radio.getCRCLength());
  printf("DynamicPayloadSize = %i \n", radio.getDynamicPayloadSize());
  printf("isPVariant = %i \n", radio.isPVariant());
  printf("PALevel = %i \n", radio.getPALevel());
  
  // TODO how to read back current address? 
  // TODO write something and see if it appears to send?

  // try to read values from the radio to determine if it is even connected, assuming these are all configured > 0
  if (radio.getCRCLength() && radio.getDynamicPayloadSize() && radio.isPVariant() && radio.getPALevel()) {
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

  //
  // Ping out role.  Repeatedly send the current time
  //
  if (role == role_ping_out) ping_out();

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //
  if ( role == role_pong_back ) pong_back();

  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

      // Become the transmitter (ping out)
      set_role_ping_out();
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");

      // Become the receiver (pong back)
      set_role_pong_back();
    }
  }
}

void ping_out() {
  // First, stop listening so we can talk.
  radio.stopListening();

  // Take the time, and send it.  This will block until complete
  unsigned long time = millis();
  printf("Now sending %lu...",time);
  bool ok = radio.write( &time, sizeof(unsigned long) );
  
  if (ok)
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
  if ( timeout )
  {
    printf("Failed, response timed out.\n\r");
  }
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );

    // Spew it
    printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    
    delay(5);
  }

  // Try again 1s later
  delay(1000);
}

void pong_back() {
  // if there is data ready
  if ( radio.available() ) {
    // Dump the payloads until we've gotten everything
    unsigned long got_time;
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got payload %lu...",got_time);
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &got_time, sizeof(unsigned long) );
    printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}

// vim:cin:ai:sts=2 sw=2 ft=cpp
