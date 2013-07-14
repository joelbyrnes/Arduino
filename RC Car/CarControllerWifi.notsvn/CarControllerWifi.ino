
#define DEBUG_ENABLED 0

#define TARGETING_ENABLED 1

/* control modes */
//#define CONTROL_MODE_HBRIDGE // use pins for left, right, back forward.
#define CONTROL_MODE_SERVO // use servo control for steer and throttle. 

#if defined(CONTROL_MODE_HBRIDGE)
#define PIN_FWD 6
#define PIN_REV 5
#define PIN_LEFT 4
#define PIN_RIGHT 7
#endif

#if defined(CONTROL_MODE_SERVO)
#define PIN_STEER 3
#define PIN_THROTTLE 2
#endif

#define PIN_PAN 4
#define PIN_TILT 5
#define PIN_SHOOT 17

#define PIN_DEBUG_TX 12
#define PIN_DEBUG_RX 13 // we don't actually receive anything.

#define PIN_V_CARBATT 14 // A0, or port 1 A

// pins 18 and 19 (a4, a5) reserved for i2c bus

/* resistor values for voltage measurement */
#define VDIV_R1 220
#define VDIV_R2 110

/* car-specific control settings */
#define THROTTLE_MAX_FORWARD 0
#define THROTTLE_MAX_REVERSE 180
#define STEER_MAX_LEFT 130
#define STEER_MAX_RIGHT 60

/* network stuff */
#define BUFSIZE 100
/*
#define PACKET_LEN_MIN 1 // a packet will be ignored if it is not at least this size
*/
#define MSGBUFSIZE 20

/* car stops if it has not received a throttle update in this time */
#define UPDATE_DELAY_MAX 300 // ms

#include <Servo.h>
#include <SoftwareSerial.h>
// currently we have to include Wire.h - can't SC16IS.h do that?
#include <Wire.h>
#include <SC16IS.h>

#include <stdio.h>
char _str[128]; // 128 chars max!  increase if required to avoid overflow
#define aprintf(...) sprintf(_str, __VA_ARGS__); Serial.println(_str)
char _dstr[128]; // 128 chars max!  increase if required to avoid overflow
#define dprintf(...) sprintf(_str, __VA_ARGS__); debug(_str)

// TODO put this in a separate file
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; while (fill < sizeof buf) buf[fill++] = NULL; fill = 0; }

    size_t write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[BUFSIZE];
};

Servo tiltServo;
Servo panServo;
Servo steerServo;
Servo throttleServo;

PacketBuffer payload;   // sending buffer
PacketBuffer targetInput;   
PacketBuffer beaconOutput;  
SC16IS uart;

long lastVRead = 0;

// holds the time the last throttle update was received. 
long lastUpdateTime = 0;

char input[BUFSIZE];
char irInput[BUFSIZE];

boolean uplinked = false;

SoftwareSerial debugSerial = SoftwareSerial(PIN_DEBUG_RX, PIN_DEBUG_TX);

void setup() {
  mainSetup();
}

void loop() {
  mainLoop();
}


