#include <TimerOne.h>
 
void setup() 
{
  // Initialize the digital pin as an output.
  pinMode(9, OUTPUT);    
  
  // period argument is optional, can be set in pwm().
  Timer1.initialize(500000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.pwm(9, 512, 100000); // pin 9, 50% duty cycle (of 1024), 100,000 us period. 

  // alternatively, you can use a service routine
  //Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}
 
void loop()
{
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
}
 
/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    // Toggle LED
    digitalWrite( 13, digitalRead( 13 ) ^ 1 );
}
