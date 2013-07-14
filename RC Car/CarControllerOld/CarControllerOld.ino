
/* jeenode available pins */
/* 
digital - D4, D5 (PWM), D6 (PWM), D7
analog  - D14/A0, D15/A1, D16/A2, D17/A3

D4     - Port1 DIO - left
D5 (PWM) - Port2 DIO - reverse
D6 (PWM) - Port3 DIO - forward
D7     - Port4 DIO - right
D14/A0 - Port1 AIO - voltage in
D15/A1 - Port2 AIO - pan
D16/A2 - Port3 AIO - tilt
D17/A3 - Port4 AIO - shoot

*/

/* digital pins */
#define PIN_PAN 15
#define PIN_TILT 16
#define PIN_FWD 6
#define PIN_REV 5
#define PIN_LEFT 4 // (used to be 8)
#define PIN_RIGHT 7
#define PIN_SHOOT 17

/* analog pins */
#define PIN_V_CARBATT 0 

#define BUFSIZE 100

#define VDIV_R1 220
#define VDIV_R2 110

#define NODE_A 1
#define NODE_B 2
#define NODE_Z 26
#define NET_BROADCAST 0
#define NODE_CONTROL 26

#include <Servo.h>
#include <RF12.h>
// stupid dependency
#include <Ports.h>

// TODO put this in a separate file
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }

#if ARDUINO < 100
    virtual void write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
#else
    virtual size_t write(uint8_t ch) {
        if (fill < sizeof buf) {
            buf[fill++] = ch;
            return 1;
        }
        return 0;
    }
#endif

private:
    byte fill, buf[RF12_MAXDATA];
};

Servo tiltServo;
Servo panServo;

PacketBuffer payload;   // sending buffer

long lastVRead = 0;

char input[BUFSIZE];
int bufIdx;

byte thisId = 0;

// status counts
int lostPacketCount = 0;

void setup() {
  pinMode(PIN_FWD, OUTPUT);
  pinMode(PIN_REV, OUTPUT);
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  pinMode(PIN_SHOOT, OUTPUT);
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
  digitalWrite(PIN_SHOOT, LOW);
  
  panServo.attach(PIN_PAN);
  tiltServo.attach(PIN_TILT);
  
  Serial.begin(57600);
  debugln("Controller ready");
  thisId = rf12_config();
  payload.reset();

  payload.print("Controller ready with ID ");
  payload.println((int) thisId);
  mustSendPayload(NET_BROADCAST); // broadcast
}

void loop() {
  processSerial();
 
  sendStatus();
}  


void sendStatus() {
  long now = millis();
  if ((now - lastVRead) > 5000) {
    long Vcc_mV = readVcc_mV();
    payload.print("Vcc: ");
    payload.println(Vcc_mV / 1000.0);
    float Vcar = getVoltage(Vcc_mV, PIN_V_CARBATT);
    payload.print("Vcar: ");
    payload.println(Vcar);
    
    payload.print("lostPackets: ");
    payload.println(lostPacketCount);
    
    if (sendPayloadIfYouCan(NODE_CONTROL) == 0) { 
      debugln("sent status.");
    } else {
      debugln("sending status failed.");
    }
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

void processSerial() {
  if (rf12_recvDone() && rf12_crc == 0) {
    //Serial.print("<");
    //Serial.print((int) RF12_HDR_MASK & rf12_hdr);
    //Serial.print("> ");    
    
    // copy packet into buffer before it disappears
    for (byte i = 0; i < rf12_len; ++i) {
      if (rf12_data[i] != '\n') {
        input[i] = rf12_data[i];
      } else {
        input[i] = '\0'; // end string
      }
    }
    
    processData(input);    
    // clean up input
    //for (int j=0; j < BUFSIZE; j++) {
    //  input[j] = '\0';
    //}
  }
}

void processData(char data[]) {
  //debug("BEHOLD: ");
  //debugln(data);
  
  char *tok = NULL;
  char *value = NULL;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "echo") == 0) {
      reply(value);
    } else if (strcmp(tok, "pan") == 0) {
      pan(value);
    } else if (strcmp(tok, "tilt") == 0) {
      tilt(value);
    } else if (strcmp(tok, "forward") == 0) {
      forward(value);
    } else if (strcmp(tok, "reverse") == 0) {
      reverse(value);
    } else if (strcmp(tok, "turn") == 0) {
      turn(value);
    } else if (strcmp(tok, "stop") == 0) {
      stopCar();
    } else if (strcmp(tok, "shoot") == 0) {
      shoot(value);
    } else {
      debug("what? : ");
      debug(tok);
      debug(" ");
      debugln(value);
    }
    tok = strtok(NULL, " \t");
  }
}

void reply(char msg[]) {
  payload.print("reply: ");
  debugln("echoing.");
  if (msg != NULL) {
    payload.println(msg);
    mustSendPayload(NODE_CONTROL);
  }
}

void pan(char value[]) {
  //debug("pan: ");
  if (value != NULL) {
    int pos = atoi(value);
    //debugln(pos, DEC);
    // set pan... 
    panServo.write(pos);   
  }
}

void tilt(char value[]) {
  //debug("tilt: ");
  if (value != NULL) {
    int pos = atoi(value);
    //debugln(pos, DEC);
    // set tilt... 
    tiltServo.write(pos);   
  }
}

void forward(char value[]) {
  //debug("forward: ");
  if (value != NULL) {
    int pct = atoi(value);
    //debugln(pct, DEC);
    goForward(pct);
  }
}

void reverse(char value[]) {
  //debug("reverse: ");
  if (value != NULL) {
    int pct = atoi(value);
    //debugln(pct, DEC);
    goBack(pct);
  }
}

void turn(char value[]) {
  //debug("turn: ");
  if (value != NULL) {
    if (value[0] == 'l') {
      goLeft();
    } else if (value[0] == 'r') {
      goRight();
    } else {
      goStraight();
    }
  }
}


void goLeft() {
  digitalWrite(PIN_RIGHT, LOW);
  digitalWrite(PIN_LEFT, HIGH);
  //debugln("left");
}

void goRight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, HIGH);
  //debugln("right");
}

void goStraight() {
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
  //debugln("straight");
}

void goForward(int amount) {
  analogWrite(PIN_REV, 0);
  analogWrite(PIN_FWD, amount);
}

void goBack(int amount) {
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, amount);
}

void stopCar() {
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  //debugln("stop");
}  

void shoot(char value[]) {
  //Serial.print("shoot: ");
  if (value != NULL) {
    //Serial.println(value);
    if (value[0] == '1') {
      digitalWrite(PIN_SHOOT, HIGH);
    } else {
      digitalWrite(PIN_SHOOT, LOW);
    }
  }
}

byte sendPayloadIfYouCan(byte dest) {
  recvAndThrowAway();
  if (rf12_canSend()) {
    _sendPayload(dest);
    return 0;
  } else {
    debugln("can't send :(");
    payload.reset();
    return 1;
  }
}

void mustSendPayload(byte dest) {
  recvAndThrowAway();
  while (!rf12_canSend()) {
    debugln("-X- can't send :( waiting...");
    //delay(2);
    recvAndThrowAway();
  }
  
  _sendPayload(dest);
}

void recvAndThrowAway() {
  if (rf12_recvDone() && rf12_crc == 0) {
    debugln("-X- OH GOD I RECEIVED AND IM IGNORING IT");
    lostPacketCount++;
  }
}

void debug(char *msg) {
  Serial.print(msg);
}
void debugln(char *msg) {
  Serial.println(msg);
}

// must have called canSend before this
void _sendPayload(byte dest) {
  // ensure payload can print correctly
  payload.print('\0');
  _sendMsg(dest, (byte *) payload.buffer(), payload.length());
  payload.reset();
}

// must have called canSend before this
void _sendMsg(byte dest, byte *msg, byte sendLen) {
  /*
  debug(" -> ");
  debug("node ");
  Serial.print((int) dest);  
  debug(", ");
  Serial.print((int) sendLen);
  debug("b : '");
  Serial.print((char *) msg);
  debug("'");
  */
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
}
