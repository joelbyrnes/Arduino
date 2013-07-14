/*
single channel voltmeter
range 0 to 5 Volts in 1023 steps
min max recorder resetable with pushbutton
LCD display driven in "4 bits" mode 
R/W operation is not used; LCD pin 5 must be connected to ground
Reset button (optional) between Arduino's digital port 6 and ground
Analog signal shall be connected to Arduino's analog port 0

no need for additional libraries

no rights, no warranty, no claim just fun
didier longueville, december 2007
*/

// from http://www.arduino.cc/playground/Main/LCDVoltmeter

// hardware related constants
#define analogPin 0
#define resetPin 6
#define ledPin 13
// lcd related constants
#define nbrCharPerLine 16 // update depending on LCD
                          // must be  at least 16 characters per line
#define nbrLines 2 // must be 2 lines at least
#define left 0
#define right 1
#define invisible 0
#define visible 1

// declare variables
#include "WProgram.h"
void setup (void);
static void nunchuck_setpowerpins();
void print(int lineIndex);
void loop (void);
void LedSendPulse(int pulseDelay);
void BufferClear ();
void BufferInsertNumValue (int digitalValue,int fullScaleValue,int decimalPlaces,int decimalSeparatorPosition);
void BufferInsertStringValue(char * s,int startingPosition);
int PowerInteger(int mantissa,int exponent);
char stringBuffer[nbrCharPerLine + 1]; // this is the working string buffer
int analogValueMax = 0;
int analogValueMin = 1023;

void setup (void) {
  Serial.begin(9600);
  
  nunchuck_setpowerpins();

  for (int i = 7;i <= 13;i++) {
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }
  pinMode(resetPin,INPUT); // define reset pin
  digitalWrite(resetPin, HIGH); // turn on pullup resistor
  
  Serial.println("SerialVoltmeter ready");
}

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize        
}

void print(int lineIndex) {
  Serial.println(stringBuffer);
}

void loop (void) {
  // check if the reset button has been pushed (quick and dirty)
  if(digitalRead(resetPin) == LOW) {
     // reset variables
    analogValueMin = 1023;
    analogValueMax = 0;
    // display status
    BufferClear(); // clear buffer
    //                       1234567890123456
    BufferInsertStringValue("min   now   max ",1);
    print(1);
    BufferClear(); // clear buffer
    //                       1234567890123456
    BufferInsertStringValue("-.--  -.--  -.--",1);
    print(2);
    delay (100); // time to read
  }
  int analogValue=analogRead(analogPin);
  analogValueMin = min(analogValue,analogValueMin); // record min value
  analogValueMax = max(analogValue,analogValueMax); // record max value
  BufferClear(); // clear buffer
  //                       1234567890123456
  BufferInsertStringValue("min   now   max ",1);
  print(1); // display converted value on line 1
  BufferClear(); // clear buffer
  //                       1234567890123456
  BufferInsertStringValue(" .     .     .  ",1);
  BufferInsertNumValue(analogValueMin,5,2,2);
  BufferInsertNumValue(analogValue,5,2,8);
  BufferInsertNumValue(analogValueMax,5,2,14);
  print(2); // display converted value on line 2
  // blink status led   
  LedSendPulse(500); 
}

/* 
led related functions:
  LedSendPulse
*/
// pulseDelay value is equal to the total pulsing time
void LedSendPulse(int pulseDelay){
  digitalWrite(ledPin,HIGH);
  delay(pulseDelay/2);
  digitalWrite(ledPin,LOW);
  delay(pulseDelay/2);  
}

/*
working string buffer functions:
  BufferClear
  BufferInsertNumValue
  BufferInsertStringValue
*/

// clears the content of the working string buffer (global variable)
void BufferClear (){
  for (int i = 1;i <= nbrCharPerLine;i++){
    stringBuffer[i - 1] = 32; // blank buffer content with space characters
  }
}

// insert converted float in the working string buffer (global variable)
void BufferInsertNumValue (int digitalValue,int fullScaleValue,int decimalPlaces,int decimalSeparatorPosition) {
  unsigned long integerValue=((unsigned long)( digitalValue * fullScaleValue) * PowerInteger(10,decimalPlaces)) / 1023;
  int remainder = 0;
  stringBuffer[decimalSeparatorPosition - 1] = 46;
  // decimals
  for (int i = 1;i <= decimalPlaces;i++){
    int asciiCode = (integerValue % 10) + 48;
    stringBuffer[decimalSeparatorPosition + decimalPlaces - i] = asciiCode;
    integerValue/=10;
  }  
  // integers
  int i = 0;
  do {
    i++;
    int asciiCode = (integerValue % 10) + 48;
    stringBuffer[decimalSeparatorPosition - 1 - i] = asciiCode;
    integerValue /= 10;
  } while (integerValue != 0);
}


// update the working string buffer (global variable)
// startingPosition is base 1
void BufferInsertStringValue(char * s,int startingPosition) {
  int stringLength=strlen(s)-1;
  for (int i = 0;i <= stringLength;i++) {
    stringBuffer[startingPosition + i - 1] = s[i];
  }
}

/*
general purpose functions:
  PowerInteger
*/

int PowerInteger(int mantissa,int exponent){
  int result; // declare result variable
  if (exponent == 0) {
    result = 1;
    }
  else {
    result=mantissa;
    for (int i = 2;i <= exponent;i++) {
      result *= mantissa;
    }
  }
  return result;
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

