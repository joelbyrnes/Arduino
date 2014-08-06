/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "printf.h"

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 0;

// Address of the other node
const uint16_t other_node = 1;

const uint16_t channel = 80;

// Structure of our payload
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(channel, /*node address*/ this_node);
  Serial.print("channel: ");
  Serial.print(channel);
  Serial.print(", this_node: ");
  Serial.println(this_node);
  
  printf_begin();
  printf("Checking radio... \n");
  if (radioConnected()) {
    printf("Radio ready.\n");
  } else {
    // probably any one of CSN (normally 10), MOSI, MISO or SCK (11, 12, 13) are not connected correctly 
    printf("Could not read from radio.\n");
  }
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // Is there anything ready for us?
  while ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.print(payload.ms);
    Serial.print(" from node ");
    Serial.println(header.from_node);
  }
}

boolean radioConnected() {
  printf("PayloadSize = %i \n", radio.getPayloadSize()); // appears to be stored in library
  printf("CRCLength = %i \n", radio.getCRCLength());
  printf("DynamicPayloadSize = %i \n", radio.getDynamicPayloadSize()); // sometimes this starts as 0
  printf("isPVariant = %i \n", radio.isPVariant());
  printf("PALevel = %i \n", radio.getPALevel());
  
  // TODO how to read back current address? 

  // try to read values from the radio to determine if it is even connected, assuming these are all configured > 0
  if (radio.getCRCLength() && radio.isPVariant() && radio.getPALevel()) {
    return true;
  } 
  return false;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
