/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Simplest possible example of using RF24Network 
 *
 * TRANSMITTER NODE
 * Every 2 seconds, send a payload to the receiver node.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <EEPROM.h>

#define DEBUG_OUTPUT
#define CHECK_RADIO

// pins for each type of microcontroller
#if defined(__AVR_ATtiny84__) // or #if defined( __AVR_ATtinyX4__ )
  #define MCU "ATtiny84"
  #define cePin 2
  #define csnPin 3
  #define txPin 1
  #define rxPin 0
  #define LED_PIN 8 
#elif defined(__AVR_ATtiny85__) // or #if defined( __AVR_ATtinyX5__ )
  #define MCU "ATtiny85"
  #define cePin 5
  #define csnPin 4
  //#define txPin 1
  //#define rxPin 0
  #define LED_PIN 8 
#elif defined(__AVR_ATmega328P__)
  #define MCU "ATmega328P"
  #define cePin 9
  #define csnPin 10
  #define LED_PIN 8
  #define txPin 1
  #define rxPin 0
#else
// ???
#endif

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(cePin, csnPin);

// Network uses that radio
RF24Network network(radio);

// Address of our node
uint16_t this_node;

// Address of the other node
const uint16_t other_node = 0;

const uint16_t channel = 90;

// How often to send 'hello world to the other unit
const unsigned long interval = 2000; //ms

// When did we last send?
unsigned long last_sent;

// How many have we sent already
unsigned long packets_sent;

// Structure of our payload
struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

void setup(void)
{
  // convenience to change node ID - keep this commented out
  EEPROM.write(0, 3);

  // wait for things to start?
  //delay(1000);
 
  pinMode(LED_PIN, OUTPUT);     
  digitalWrite(LED_PIN, LOW);  

  SPI.begin();
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.powerUp();
    
  this_node = EEPROM.read(0); 

  #if defined(DEBUG_OUTPUT)
  Serial.begin(115200);
  Serial.print("helloworld_tx85 ");
  Serial.println(MCU);
  Serial.print("ch: ");
  Serial.print(channel);
  Serial.print(", node: ");
  Serial.println(this_node);
  #endif  

  #if defined(CHECK_RADIO) 
  if (radioConnected()) {
    #if defined(DEBUG_OUTPUT)
    Serial.println("+++ Radio ready.");
    #endif 
  } else {
    // probably any one of CSN (normally 10), MOSI, MISO or SCK (11, 12, 13) are not connected correctly
    #if defined(DEBUG_OUTPUT) 
    Serial.println("--- No radio.");
    #endif 
    digitalWrite(LED_PIN, HIGH);  
    delay(100);
  }
  #endif  
  
  network.begin(channel, this_node);
}

void loop(void)
{
  //Serial.println("loop.");
  // Pump the network regularly
  network.update();

  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    last_sent = now;

#if defined(DEBUG_OUTPUT)
    //Serial.print("Sending...");
#endif
    payload_t payload = { millis(), packets_sent++ };
    
    RF24NetworkHeader header(/*to node*/ other_node);
    
#if defined(DEBUG_OUTPUT)
    Serial.print("tx ");
    Serial.print(payload.ms);
    Serial.print(", ");
    Serial.print(payload.counter);
    Serial.print("; ");
    Serial.print(this_node);
    Serial.print(" -> ");
    Serial.println(other_node);
#endif

    bool ok = network.write(header,&payload,sizeof(payload));

    if (ok) {
//      last_sent = now;
      digitalWrite(LED_PIN, LOW);  
#if defined(DEBUG_OUTPUT)
      Serial.println("ok");
#endif
    } else {
      digitalWrite(LED_PIN, HIGH);  
#if defined(DEBUG_OUTPUT)
      Serial.println("no");
#endif
    }
  }
}

boolean radioConnected() {
//  printf("PayloadSize = %i \n", radio.getPayloadSize()); // appears to be stored in library
//  printf("CRCLength = %i \n", radio.getCRCLength());
//  printf("DynamicPayloadSize = %i \n", radio.getDynamicPayloadSize()); // sometimes this starts as 0
//  printf("isPVariant = %i \n", radio.isPVariant());
//  printf("PALevel = %i \n", radio.getPALevel());
  
  // TODO how to read back current address? 

  // try to read values from the radio to determine if it is even connected, assuming these are all configured > 0
  if (radio.getCRCLength() && radio.isPVariant() && radio.getPALevel()) {
    return true;
  } 
  return false;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
