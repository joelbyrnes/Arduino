//This example demonstates the use of the ULN2003 to drive a stepper motor.
//The speed and direction of the stepper motor is determined by adjusting
//the potentiometer connected to A2. When the potentiometer is rotated fully
//counterclockwise, the motor will rotate at full counterclockwise speed. As
//the potentiometer is rotated clockwise, the motor will continue to slow down
//until is reaches its minimum speed (when the potentiometer is at the middle
//position.) Once the potentiometer crosses the middle position, the motor will
//reverse direction. As the potentiomer is rotated further clockwise, the speed
//of the motor will increase until it reaches its full clockwise rotation speed
//when the potentiometer has been rotated fully clockwise.

//////////////////////////////
////////////////////////////////////////////////
//declare variables for the motor pins

int motorPin4 = 5;
int motorPin3 = 6;
int motorPin2 = 7;
int motorPin1 = 8;

int motorSpeed = 0; //variable to set stepper speed
int potPin = 2; //potentiometer connected to A2
int potValue = 0; //variable to read A0 input

//////////////////////////////////////////////////////////////////////////////
void setup() {
//declare the motor pins as outputs
//int potValue = potValue*2;
pinMode(motorPin1, OUTPUT);
pinMode(motorPin2, OUTPUT);
pinMode(motorPin3, OUTPUT);
pinMode(motorPin4, OUTPUT);
Serial.begin(9600);
}

//////////////////////////////////////////////////////////////////////////////
void loop(){

potValue = analogRead(potPin); //read the value of the potentiometer
Serial.println(potValue);//View full range from 0 - 1023 in Serial Monitor
if (potValue < 511){ //if potentiometer reads 0 to 511 do this
motorSpeed = (potValue/15 + 8); //scale potValue to be useful for motor
clockwise(); //go to the ccw rotation function
}
else { //value of the potentiometer is 511 - 1023
motorSpeed = ((1023-potValue)/15 + 8); //scale potValue for motor speed
counterclockwise(); //go the the cw rotation function
}
}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 1 to 4
//delay "motorSpeed" between each pin setting (to determine speed)

void counterclockwise (){
digitalWrite(motorPin1, HIGH);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin4, LOW);
delay(motorSpeed);
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, HIGH);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin4, LOW);
delay (motorSpeed);
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin3, HIGH);
digitalWrite(motorPin4, LOW);
delay(motorSpeed);
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin4, HIGH);
delay(motorSpeed);
}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 4 to 1
//delay "motorSpeed" between each pin setting (to determine speed)

void clockwise(){
digitalWrite(motorPin4, HIGH);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin1, LOW);
delay(motorSpeed);
digitalWrite(motorPin4, LOW);
digitalWrite(motorPin3, HIGH);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin1, LOW);
delay (motorSpeed);
digitalWrite(motorPin4, LOW);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin2, HIGH);
digitalWrite(motorPin1, LOW);
delay(motorSpeed);
digitalWrite(motorPin4, LOW);
digitalWrite(motorPin3, LOW);
digitalWrite(motorPin2, LOW);
digitalWrite(motorPin1, HIGH);
delay(motorSpeed);
}
