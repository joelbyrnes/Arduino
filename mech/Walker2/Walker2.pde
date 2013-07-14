
#include <Servo.h> 

#define BUFSIZE 100
#define DEBUG_ENABLED 0

// movement - reach an angle target in X time. 
// either store angle and target time and figure out the increment on the fly,
// or store the angle and an increment for standard movement speed, handles delays without jerking. 
class Movement {
  public:
    Movement () {
      done = 1;
    }
      
    void init(unsigned long milliSecs, double fromAngle, double toAngle) {
      time = milliSecs;
      startAngle = fromAngle;
      maxAngle = toAngle;
      angleDiff = maxAngle - startAngle;
      done = 0;  
    
      startTime = millis();
    }  
    
    double getAngle() {
      // if this is called when alredy done then just give the final angle
      if (done) return maxAngle;
      
      unsigned long diffTime = millis() - startTime;
      
      // never return greater than the max angle, and set done if it is time. 
      if (diffTime >= time) {
        done = 1;
        return maxAngle;
      }
      return (double(diffTime) / double(time)) * angleDiff + startAngle;
    }
    
    boolean isDone() {
      return done;
    }
  
  private:
    unsigned long startTime, time;  
    double startAngle, maxAngle, angleDiff;
    boolean done;
};

Movement hipMovement; 


char input[BUFSIZE];

Servo hipServo; 
Servo kneeServo; 
Servo footServo; 

double hipPos = 90.0;
double kneePos = 135.0;
double footPos = 90.0;

void setup() 
{ 
  Serial.begin(57600);
  hipServo.attach(2); 
  kneeServo.attach(3); 
  footServo.attach(4); 

  // standing starting position
  initPosition();
  
  // TODO is there a servo where we can find the current angle?
  
  // hip - max 135 (crouching), min 45 
  // knee - max 135 (crouching), min 45
  // foot - max 135 (crounching, min 45   ???
  
  Serial.println("Mech ready");
} 

void initPosition() {
  // this kind of sudden change will jerk around, but we have no way to know the starting position. 
  hipPos = 90.0;
  kneePos = 135.0;
  footPos = 90.0;
  
  writePos();
}
 
void loop() { 
  //processSerial();
  testMovement();
  
  movement();
  
  //verticalTest();
}

unsigned long startTime;
unsigned long secs;
double startAngle, maxAngle, angleDiff;
int done = 1;

void initMovement(unsigned long time, double fromAngle, double toAngle) {
  secs = time;
  startAngle = fromAngle;
  maxAngle = toAngle;
  angleDiff = toAngle - fromAngle;
  done = 0;  

  startTime = millis();
}  

double getAngle() {
  unsigned long diffTime = millis() - startTime;
  return (double(diffTime) / double(secs)) * angleDiff + startAngle;
}

void movement() {
  if (done) return;
  
  // get the angle, check if we've reached the target angle already. 
  hipPos = getAngle();
  if (hipPos >= maxAngle) {
    hipPos = maxAngle;
    done = 1;
  }

  writePos();
}

int testDirection = 0;

void movementClass() {
  if (hipMovement.isDone()) return;

  hipPos = hipMovement.getAngle();
  writePos();
}

void testMovementClass() {
  // move from 90 to 135 in 2000ms
  if (hipMovement.isDone()) {
    // reset
    if (testDirection = 0) {
      // start position to full extension
      hipMovement.init(2000, 90.0, 135.0);
      testDirection = 1;
    } else {
      // back to start position
      hipMovement.init(2000, 135.0, 90.0);
      testDirection = 0;
    }
  }  
  
  hipMovement.init(2000, 90.0, 135.0);
  hipPos = hipMovement.getAngle();
}

void testMovement() {
  // move from 90 to 135 in 2000ms
  if (done) {
    // reset
    if (testDirection = 0) {
      // start position to full extension
      initMovement(2000, 90.0, 135.0);
      testDirection = 1;
    } else {
      // back to start position
      initMovement(2000, 135.0, 90.0);
      testDirection = 0;
    }
  }
}
  
void servoTest() {
  for (double i = 45.0; i < 135; i += 0.10) {
    hipPos = i;
    writePos();
    delay(2);
  }
  for (double i = 135.0; i > 45; i -= 0.10) {
    hipPos = i;
    writePos();
    delay(2);
  }
}

// this is the naive case where we assume limb lengths and angles and balance.
void verticalTest() {
  // do a full extend
  for (double i = 0; i < 4.5; i += 0.10) {
    vertical(i);
    delay(50);
  }

  // do a full crouch
  for (double j = 4.5; j > 0; j -= 0.10) {
    vertical(j);
    delay(50);
  }
} 

void horizontalTest() {  
  // stretch
  for (int i = 0; i < 20; i++) {
    horizontal(1);
    delay(200);
  }

  // crouch
  for (int j = 0; j < 20; j++) {
    vertical(-1);
    delay(200);
  }
} 

// this is the naive case where we assume limb lengths and angles and balance.
// push upwards or pull downwards
void vertical(double amt) {
  hipPos += amt;
  footPos += amt;
  kneePos -= 2 * amt;
  
  writePos();
}

// this is the naive case where we assume limb lengths and angles and balance.
// push backwards from foot or pull forwards - positive is hip forward, neg is back. 
void horizontal(double amt) {
  // don't change the knee angle, just the hip and foot angles relative to one another
  
  hipPos += amt;
  footPos -= amt;
  
  writePos();
}

void writePos() {
  Serial.print("h: \t");
  Serial.print(hipPos);
  Serial.print(", \tk: \t");
  Serial.print(kneePos);
  Serial.print(", \tf: \t");
  Serial.println(footPos);
  
  if (hipServo >= 45.0 && hipServo =< 135.00)
    hipServo.write(hipPos);
  if (kneeServo >= 45.0 && kneeServo =< 135.00)
    kneeServo.write(kneePos);
  if (footServo >= 45.0 && footServo =< 135.00)
    footServo.write(footPos);
}






int n;

void processSerial() {
  // to swap between serials just switch out uart for Serial
  
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      debugln("overflow! discarding command. ");
      n = 0;
      break;
    }
    char in = Serial.read();
    if (in != '\n') {
      input[n++] = in;
    } else {
      input[n] = '\0'; // end string
      processData(input);
      n = 0;
    }
  }
}

void processData(char input[]) {
  debug("received data: ");
  debugln(input);

  processCommand(input); 
}

void processCommand(char data[]) {
  char *tok = NULL;
  char *value = NULL;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "echo") == 0) {
      reply(value);
    } else if (strcmp(tok, "h") == 0) {
      hip(value);
    } else if (strcmp(tok, "k") == 0) {
      knee(value);
    } else if (strcmp(tok, "f") == 0) {
      foot(value);
    } else if (strcmp(tok, "m") == 0) {
      char *fromAngle = strtok(NULL, " \t");
      char *toAngle = strtok(NULL, " \t");
      move(value, fromAngle, toAngle);
    } else {
      debug("what? : ");
      debug(tok);
      debug(" ");
      debug(value);
      debugln("");
    }
    tok = strtok(NULL, " \t");
  }
}

void reply(char msg[]) {
  Serial.print("reply: ");
  debug("echoing. ");
  if (msg != NULL) {
    debugln(msg);
    Serial.println(msg);
  }
}

void hip(char value[]) {
  //debug("hip: ");
  if (value != NULL) {
    double pos = atof(value);
    //debugln(pos, DEC);
    // set pan... 
    hipServo.write(pos);   
  }
}

void knee(char value[]) {
  //debug("knee: ");
  if (value != NULL) {
    double pos = atof(value);
    //debugln(pos, DEC);
    // set pan... 
    kneeServo.write(pos);   
  }
}

void foot(char value[]) {
  //debug("foot: ");
  if (value != NULL) {
    double pos = atof(value);
    //debugln(pos, DEC);
    // set pan... 
    footServo.write(pos);   
  }
}

void move(char timeStr[], char fromStr[], char toStr[]) {
  //debug("foot: ");
  if (timeStr != NULL && fromStr != NULL && toStr != NULL) {
    unsigned long timeVal = atoi(timeStr);
    double fromVal = atof(fromStr);
    double toVal = atof(toStr);
    //debugln(pos, DEC);
    hipMovement.init(timeVal, fromVal, toVal);
  }
}


void debug(char *msg) {
#if DEBUG_ENABLED
  if (msg == NULL) return;
  //debugSerial.print(msg);
  Serial.print(msg);
#endif
}
void debugln(char *msg) {
#if DEBUG_ENABLED
  if (msg == NULL) return;
  //debugSerial.println(msg);
  Serial.println(msg);
#endif
}

