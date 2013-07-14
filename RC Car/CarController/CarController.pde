
/* jeenode available pins */
/* 
digital - D4, D5 (PWM), D6 (PWM), D7
analog  - D14/A0, D15/A1, D16/A2, D17/A3

D4     - Port1 DIO - steering servo
D5 (PWM) - Port2 DIO - pan servo
D6 (PWM) - Port3 DIO - tilt servo
D7     - Port4 DIO - throttle servo
D14/A0 - Port1 AIO - car batt voltage in
D15/A1 - Port2 AIO - debug TX
D16/A2 - Port3 AIO - 
D17/A3 - Port4 AIO - shoot

*/

/* control modes */
#define CONTROL_MODE_HBRIDGE // use pins for left, right, back forward.
//#define CONTROL_MODE_SERVO // use servo control for steer and throttle. 

#if defined(CONTROL_MODE_HBRIDGE)
#define PIN_FWD 6
#define PIN_REV 5
#define PIN_LEFT 4
#define PIN_RIGHT 7
#endif

#if defined(CONTROL_MODE_SERVO)
#define PIN_STEER 4
#define PIN_THROTTLE 7
#endif

#define PIN_PAN 15
#define PIN_TILT 16
#define PIN_SHOOT 17

#define PIN_DEBUG_TX 18
#define PIN_DEBUG_RX 19 // we don't actually receive anything.

#define PIN_V_CARBATT 14 // A0, or port 1 A

/* resistor values for voltage measurement */
#define VDIV_R1 220
#define VDIV_R2 110

/* car-specific control settings */
#define THROTTLE_MAX_FORWARD 0
#define THROTTLE_MAX_REVERSE 180
#define STEER_MAX_LEFT 130
#define STEER_MAX_RIGHT 50

/* network stuff */
#define BUFSIZE 100
#define PACKET_LEN_MIN 1 // a packet will be ignored if it is not at least this size

#define NODE_A 1
#define NODE_B 2
#define NODE_Z 26
#define NET_BROADCAST 0
#define NODE_CONTROL 26

/* car stops if it has not received a throttle update in this time */
#define UPDATE_DELAY_MAX 300 // ms

#define DEBUG_ENABLED 0

#include <Servo.h>
#include <RF12.h>
// stupid dependency
#include <Ports.h>
#include <SoftwareSerial.h>

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

PacketBuffer payload;   // sending buffer

long lastVRead = 0;

// holds the time the last throttle update was received. 
long lastUpdateTime = 0;

char input[BUFSIZE];
int bufIdx;

byte thisId = 0;

boolean uplinked = false;
// status counts
int droppedPackets = 0;
int sendTimeoutCount = 0;

SoftwareSerial debugSerial = SoftwareSerial(PIN_DEBUG_RX, PIN_DEBUG_TX);

void setup() {
  mainSetup();
}

void loop() {
  mainLoop();
}


