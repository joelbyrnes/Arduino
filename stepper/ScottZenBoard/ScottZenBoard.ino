/*
  Program for Scott Moorhead's "zen" board

*/

int motorPin1 = 10;
int motorPin2 = 11;
int motorPin3 = 12;
int motorPin4 = 13;

int coilTime = 3000; //variable to set stepper speed

int motorSpeed = coilTime; 

//////////////////////////////////////////////////////////////////////////////
void setup() {
//declare the motor pins as outputs
pinMode(motorPin1, OUTPUT);
pinMode(motorPin2, OUTPUT);
pinMode(motorPin3, OUTPUT);
pinMode(motorPin4, OUTPUT);
  delay(500);
}

//////////////////////////////////////////////////////////////////////////////
void loop(){
  
  // accelerate to middle of steps, decelerate to end

  // stepper is 32 with a 16x gear ratio - so 512 steps in total.
  // however each call steps 4 times - so is it actually a 64 gear ratio?

  int steps = 128; // 90 degrees
  
  /*
  for (int i=0; i< steps; i++) {
//    motorSpeed = int(coilTime * (float(steps)/i));
//    clockwise(1); 
    clockwiseSteps(4);
  }
  //off();
  
  */

/*
  // accel
  motorSpeed = int(coilTime * 2);
  clockwiseSteps(16);
  motorSpeed = int(coilTime * 1.5);
  clockwiseSteps(16);
  motorSpeed = int(coilTime * 1.0);
  clockwiseSteps(32);
  motorSpeed = int(coilTime * 0.7);
  clockwiseSteps(32);

  motorSpeed = int(coilTime * 0.5);
  clockwiseSteps(64);

  // decel
  motorSpeed = int(coilTime * 0.7);
  clockwiseSteps(32);
  motorSpeed = int(coilTime * 1.0);
  clockwiseSteps(32);
  motorSpeed = int(coilTime * 1.5);
  clockwiseSteps(16);
  motorSpeed = int(coilTime * 2);
  clockwiseSteps(8);
  motorSpeed = int(coilTime * 3);
  clockwiseSteps(4);
  motorSpeed = int(coilTime * 4);
  clockwiseSteps(4);
  */

  // 90 degree version  
  motorSpeed = int(coilTime * 2);
  clockwiseSteps(8);
  motorSpeed = int(coilTime);
  clockwiseSteps(88);

  // decel
  motorSpeed = int(coilTime * 1.5);
  clockwiseSteps(16);
  motorSpeed = int(coilTime * 2);
  clockwiseSteps(8);
  motorSpeed = int(coilTime * 3);
  clockwiseSteps(4);
  motorSpeed = int(coilTime * 4);
  clockwiseSteps(4);
  
  delay(500);
  
  /*
  for (int i=0; i< steps; i++) {
    counterclockwise(1); 
  
  }
  //off();
  delay(500);
*/

}

void accelerate(int steps, float startRatio, float endRatio) {
  for (int i=0; i< steps; i++) {
    // pct there = i / steps * 100%
    // pct * 


    motorSpeed = int(float(coilTime) * (float(i)/float(steps)));
    clockwise(1); 
  }
}

void clockwiseSteps(int steps) {
  for (int i=0; i< steps; i++) {
//    //motorSpeed = int(coilTime * (float(steps)/i));
    clockwise(1); 
  }
}


// half wave uses more power (50%) but delivers more torque
void counterclockwise (int halfwave){
  if (halfwave) {
    output(0,0,1,1);
    output(0,1,1,0);
    output(1,1,0,0);
    output(1,0,0,1);
  } else {
    output(0,0,0,1);
    output(0,0,1,0);
    output(0,1,0,0);
    output(1,0,0,0);
  }
}

void clockwise(int halfwave){
  if (halfwave) {
    // as suggested somewhere - seems to work best even with less delay between
    output(1,1,0,0);
    output(0,1,1,0);
    output(0,0,1,1);
    output(1,0,0,1);

    // as done in Stepper library - didn't work for me or this stepper http://www.tigoe.net/pcomp/code/circuits/motors/stepper-motors/
    /* 
    output(1,0,1,0);
    output(0,1,1,0);
    output(0,1,0,1);
    output(1,0,0,1);
    */
  } else { // quarter wave
    output(1,0,0,0);
    output(0,1,0,0);
    output(0,0,1,0);
    output(0,0,0,1);
  }    

}

void off (){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

void output(int out1, int out2, int out3, int out4) {
  digitalWrite(motorPin1, out1);
  digitalWrite(motorPin2, out2);
  digitalWrite(motorPin3, out3);
  digitalWrite(motorPin4, out4);
  delayMicroseconds(motorSpeed);
}
