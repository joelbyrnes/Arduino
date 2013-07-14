
#define NODE_A 1
#define NODE_B 2
#define NODE_Z 26
#define NET_BROADCAST 0
#define NODE_CONTROL 26

#include <Ports.h>
#include <RF12.h> // needed to avoid a linker error :(

Port one (1);
Port two (2);
Port three (3);
Port four (4);

MilliTimer timer;

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

PacketBuffer payload;   // sending buffer

//char input[BUFSIZE];

void setup() {
  one.mode(INPUT);
  two.mode(INPUT);
  three.mode(INPUT);
  four.mode(INPUT);
      
  Serial.begin(57600);
  Serial.println("\n[wheel]");
  
  rf12_config();

}

void loop() {
  int wheelVal = one.anaRead();
  int pedalVal = two.anaRead();
  int rightVal = three.digiRead(); // right wheel button
  int leftVal = four.digiRead(); // left wheel button
  int gearUpVal = one.digiRead(); 
  int gearDownVal = two.digiRead(); 
    
  if (timer.poll(10)) {
    printStates(wheelVal, pedalVal, rightVal, leftVal, gearUpVal, gearDownVal);
    sendStates(wheelVal, pedalVal, rightVal, leftVal, gearUpVal, gearDownVal);
  }
  
}

byte mapToByte(int minVal, int maxVal, int val) {
  float range = maxVal - minVal;
  float diff = val - minVal;
  int result = (diff / range) * 255;
  Serial.print("range: ");
  Serial.print(range);
  Serial.print(" diff: ");
  Serial.print(diff);
  Serial.print(" result: ");
  Serial.println(result);
  
  if (result > 255) result = 255;
  return result;
}

// pots
int lastWheelVal, lastPedalVal = 0;
// buttons are inverted
int lastRightVal, lastLeftVal, lastGearUpVal, lastGearDownVal = 1;

void sendStates(int wheelVal, int pedalVal, int rightVal, int leftVal, int gearUpVal, int gearDownVal) {
  // TODO read gear and set in forward or reverse mode? then other pedal is just stop. 
  
  if (pedalVal != lastPedalVal) {
    // normal 96-106, max forward 512, max brake 56. both down can be balanced at ~100. 
    if (pedalVal > 110) {
      payload.print("forward\t");
      payload.print(int(mapToByte(100, 350, pedalVal)));
      payload.print("\t");
    } else if (pedalVal < 90) {
      payload.print("reverse\t");
      payload.print(int(mapToByte(100, 60, pedalVal)));
      payload.print("\t");
    } else {
      payload.print("stop\t0\t");
    }
    lastPedalVal = pedalVal;
  }
  
  if (wheelVal != lastWheelVal) {
    // center ~277-320, max left 1023, max right 173
    // leave a deadzone in the middle
    if (wheelVal > 360) {
      payload.print("turn\tl\t");
    } else if (wheelVal < 230) {
      payload.print("turn\tr\t");
    } else {
      payload.print("turn\tnone\t");
    }
    lastWheelVal = wheelVal;
  }
  
  if (rightVal != lastRightVal) {
    if (rightVal == 0) {
      payload.print("shoot\t1\t");
    } else {
      payload.print("shoot\t0\t");
    }
  }
    
  payload.println();
  
  if (sendPayloadIfYouCan(NODE_A) == 0) { 
    debugln("sent status.");
  } else {
    debugln("sending status failed.");
  }
}
  
  
void printStates(int wheelVal, int pedalVal, int rightVal, int leftVal, int gearUpVal, int gearDownVal) {
    Serial.print("wheel: ");
    Serial.print(wheelVal);
    Serial.print(" \t");

    Serial.print("pedal: ");
    Serial.print(pedalVal);
    Serial.print(" \t");

    Serial.print("gear up: ");
    // buttons are shorted to Gnd when pressed
    if (gearUpVal == 0) {
      Serial.print("ON ");
    } else {
      Serial.print("OFF ");
    }

    Serial.print("gear down: ");
    // buttons are shorted to Gnd when pressed
    if (gearDownVal == 0) {
      Serial.print("ON ");
    } else {
      Serial.print("OFF ");
    }

    printButtonState("gear up", gearUpVal);
    printButtonState("gear down", gearDownVal);
    printButtonState("right", rightVal);
    printButtonState("left", leftVal);
    
    Serial.println();
}

void printButtonState(char *name, int value) {
    Serial.print(name);
    Serial.print(": ");
    // buttons are shorted to Gnd when pressed
    if (value == 0) {
      Serial.print("ON ");
    } else {
      Serial.print("OFF ");
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
  rf12_recvDone();
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
  
  debug(" -> ");
  debug("node ");
  Serial.print((int) dest);  
  debug(", ");
  Serial.print((int) sendLen);
  debug("b : '");
  Serial.print((char *) msg);
  debug("'");
  
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
}


