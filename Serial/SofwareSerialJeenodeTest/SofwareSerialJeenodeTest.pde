
#define PIN_DEBUG_TX 18
#define PIN_DEBUG_RX 19 // we don't actually receive anything. B1 - not used.

#define DEBUG_ENABLED 1

#define VDIV_R1 220
#define VDIV_R2 110

#include <Servo.h>
#include <RF12.h>
// stupid dependency
#include <Ports.h>
#include <SoftwareSerial.h>

SoftwareSerial debugSerial = SoftwareSerial(PIN_DEBUG_RX, PIN_DEBUG_TX);

MilliTimer timer;

void setup() {
  pinMode(PIN_DEBUG_TX, OUTPUT);
  pinMode(PIN_DEBUG_RX, INPUT);

  Serial.begin(4800);
  debugSerial.begin(4800);

  rf12_config();
  Serial.println("hardware serial");
  debugSerial.println("software serial");
  debugln("Controller ready");
}

void loop() {
  if (timer.poll(500)) {
    debugln("testicle??!");
    
    sendStatus();
  }
}  


// TODO put this in a separate file
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }

    virtual void write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[RF12_MAXDATA];
};

Servo tiltServo;
Servo panServo;
Servo steerServo;
Servo throttleServo;

long lastVRead = 0;
PacketBuffer payload;   // sending buffer

void sendStatus() {
  long now = millis();
  if ((now - lastVRead) > 5000) {
    long Vcc_mV = readVcc_mV();
    payload.print("Vcc: ");
    payload.println(Vcc_mV / 1000.0);
    float Vcar = getVoltage(Vcc_mV, 14);
    payload.print("Vcar: ");
    payload.println(Vcar);
    
    // spend up to 50ms trying to send status
    //if (sendPayloadWithinTime(NODE_CONTROL, 50) == 0) { 
      debugln("sent status.");
    //} else {
    //  debugln("sending status failed.");
    //}
    lastVRead = now;
  }
}

float getVoltage(long Vcc_mV, int analogPin) {
  float raw = analogRead(analogPin);
  float Vin = (Vcc_mV / 1000.0 / 1023.0) * raw; // Calc Voltage with reference to Vcc
  float factor = (VDIV_R1 + VDIV_R2) / VDIV_R2;
  float Vcar = factor * Vin;
  return Vcar;
}

long readVcc_mV() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void debug(char *msg) {
#if DEBUG_ENABLED
  debugSerial.print(msg);
  Serial.print(msg);
#endif
}
void debugln(char *msg) {
#if DEBUG_ENABLED
  debugSerial.print("debug: ");
  debugSerial.println(msg);
  Serial.print("hardware serial: ");
  Serial.println(msg);
#endif
}


