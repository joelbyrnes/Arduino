#include <Ports.h>
#include <RF12.h>
#include <RF12sio.h>

#define NODE_A 1
#define NODE_B 2
#define NODE_Z 26
#define NET_BROADCAST 0
#define NODE_CONTROL 26

#define UPDATE_INTERVAL 60000

#define KELVIN_OFFSET 273.0
// node 1
#define OFFSET 45
// node 2
//#define OFFSET 50.0

#define GAIN   1.22

// 1 28 = 81
// 1 29.2 = 82
// 1 64 = 124
// 1 42 = 97

// 2 25.0 = 86
// 2 49 = 116
// 2 41 = 105
// 2 30 = 89,90
// 2 29 = 89
// 2 30.7 = 82

RF12 RF12;

class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }

    size_t write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[RF12_MAXDATA];
};

byte myId;
PacketBuffer payload;   // sending buffer

// enables low power sleep mode
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// not sure how this works, turns off other interrupts so serial.print doesn't break sleep?
// http://forum.jeelabs.net/node/780
ISR(PCINT2_vect) {}

void setup()
{
  Serial.begin(9600);
  payload.print("\0");
  payload.reset();
  Serial.println("Internal Temperature Sensor");
  myId = rf12_config();
  delay(500);
}

void loop() {
  double temp = GetTemp();

  payload.print("Node ");
  payload.print(myId);
  payload.print(" Temp ");
  payload.print(temp);
  payload.print(" C");
  
  // must recv to clear buffers or something
  while (!rf12_canSend())
    rf12_recvDone();
  
  sendPayload(NET_BROADCAST);
  rf12_sendWait(1);
  
  // put the radio to sleep, go to low power mode for a time, wake radio up again. 
  rf12_sleep(RF12_SLEEP);
  Sleepy::loseSomeTime(UPDATE_INTERVAL);
  rf12_sleep(RF12_WAKEUP);
}

// must have called canSend before this
void sendPayload(byte dest) {
  // ensure payload can print correctly
  payload.print('\0');
  sendMsg(dest, (byte *) payload.buffer(), payload.length());
  payload.reset();
}

// must have called canSend before this
void sendMsg(byte dest, byte *msg, byte sendLen) {
  Serial.print(" -> ");
  Serial.print("node ");
  Serial.print((int) dest);  
  Serial.print(", ");
  Serial.print((int) sendLen);
  Serial.print("b: '");
  Serial.print((char*) msg);
  Serial.println("'");
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
}

double GetTemp(void)
{
  unsigned int wADC;
  double t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
//  t = (double(wADC) - double(324.31) ) / 1.22;
  t = (double(wADC) - KELVIN_OFFSET - OFFSET) / GAIN;

  // The returned temperature is in degrees Celcius.
  return t;
}
