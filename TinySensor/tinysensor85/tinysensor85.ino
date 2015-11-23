#include <JeeLib.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <EEPROM.h>
#include <DHT.h>

#include "tinysensor.h"

#define DEBUG_OUTPUT
#define DEBUG_PAYLOAD

// pins for each type of microcontroller
#if defined(__AVR_ATtiny84__)
  #define MCU "ATtiny84"
  #define batPin A0
  #define lightPin A1
  #define dhtPin A2
  #define cePin 2
  #define csnPin 3
  #define txPin 1
  #define rxPin 0
#elif defined(__AVR_ATtiny85__)
  #define MCU "ATtiny85"
  //#define batPin 5
  //#define lightPin 2
  #define dhtPin 3
  #define cePin 5
  #define csnPin 4
  //#define txPin 1
  //#define rxPin 0
#elif defined(__AVR_ATmega328P__)
  #define MCU "ATmega328P"
  #define batPin A2
  #define lightPin A3
  #define dhtPin 4
  #define irqPin 2 // just to reserve it
  #define cePin 9
  #define csnPin 10
  #define txPin 1
  #define rxPin 0
#else
// ???
#endif

RF24 radio(cePin, csnPin);
RF24Network network(radio);
DHT dht;

const uint16_t master_node = 0;
uint16_t this_node;

const uint16_t channel = 90;

const unsigned long interval = 6000; //ms

void setup(void)
{
  // for LDR
#if !defined(__AVR_ATtiny85__)
  pinMode(lightPin, INPUT);
  digitalWrite(lightPin, HIGH);   // enable pullup
#endif

  dht.setup(dhtPin);
  
  SPI.begin();
  radio.begin();
  
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);

  this_node = EEPROM.read(0);
  network.begin(channel, this_node);
  
  #if defined(DEBUG_OUTPUT)
  Serial.begin(115200);
  Serial.print("tinysensor (ATtiny85 compatible) on ");
  Serial.println(MCU);
  Serial.print("channel: ");
  Serial.print(channel);
  Serial.print(", this_node: ");
  Serial.println(this_node);
  #endif
  
  #if defined(DEBUG_OUTPUT) && defined(DEBUG_PAYLOAD)
  Serial.println("Millis\tStatus\tHumidity\tTemperature\tLight\tBattery\tID");
  #endif
  
  delay(dht.getMinimumSamplingPeriod());
}

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void loop(void)
{
  radio.powerUp();
  network.update();  

#if defined(__AVR_ATtiny85__)
  uint8_t light = 255;
  uint16_t battery = 255;
#else
  uint8_t light = 255 - analogRead(lightPin) / 4;
  uint16_t battery = analogRead(batPin);
#endif

  int16_t h,t;
  
  h = dht.getHumidity();
  t = dht.getTemperature();
  if (h == 0) {
    // bad read, try again
    
    h = dht.getHumidity();
    t = dht.getTemperature();
  }

  sensor_payload_t payload = { millis(), light, dht.getStatus(), h, t, battery };
  RF24NetworkHeader header(master_node, sensor_type_id);
  
  #if defined(DEBUG_OUTPUT) && defined(DEBUG_PAYLOAD)
  Serial.print(payload.ms);
  Serial.print("\t");
  Serial.print(payload.status);
  Serial.print("\t");
  Serial.print(payload.humidity);
  Serial.print("\t\t");
  Serial.print(payload.temperature);
  Serial.print("\t\t");
  Serial.print(payload.light);
  Serial.print("\t");
  Serial.print(payload.battery);
  Serial.print("\t");
  Serial.println(header.from_node);  
  #endif
  
  bool ok = network.write(header, &payload, sizeof(payload));
  #if defined(DEBUG_OUTPUT)
  if (ok) Serial.println("ok.");
  else Serial.println("failed.");
  #endif

  radio.powerDown();

  int32_t l = light;
  int32_t s = 1000 * (255 - l) / 3;
  if (s < 15000)
    s = 15000;
  do {
    int32_t i = min(s, interval);  
    Sleepy::loseSomeTime(i);
    s -= i;
  } while (s > 0);
}
