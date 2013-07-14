
// movement - reach an angle target in X time. 
// either store angle and target time and figure out the increment on the fly,
// or store the angle and an increment for standard movement speed, handles delays without jerking. 

/*
class Movement {
public:
    Movement () : fill (0) {}
    
    byte length() { return fill; }
    void reset() { fill = 0; }

    virtual void write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    double start, target, increment;
};

Movement kneeMovement; 
*/

#include <Servo.h> 
 
Servo hip; 
Servo knee; 
Servo foot; 

double hipPos = 90.0;
double kneePos = 135.0;
double footPos = 90.0;

void setup() 
{ 
  Serial.begin(57600);
  hip.attach(2); 
  knee.attach(3); 
  foot.attach(4); 

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
  //verticalTest();
  //servoTest();
  
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
  Serial.print("setting hip: \t");
  Serial.print(hipPos);
  Serial.print(", \tknee: \t");
  Serial.print(kneePos);
  Serial.print(", \tfoot: \t");
  Serial.println(footPos);
  
  hip.write(hipPos);
  knee.write(kneePos);
  foot.write(footPos);
}





