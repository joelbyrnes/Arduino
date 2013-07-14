
// pin state change interrupt based IR receiver
// observing the IR pin changes, measure time between interrupts
// use state machine to decide what to do

unsigned long elapsedSince(unsigned long since, unsigned long now) {
  return since < now ? now-since : 0xFFFFFFFFUL - (now - since);
}

unsigned long elapsedSince(unsigned long since) {
  return elapsedSince( since, micros() );
}

// iSOBOT IR protocol timing
#define TimeStart 2500
#define TimeZero  500
#define TimeOne   1000
#define TimeOff   500

// check if the time was in range +/- 25%
#define IS_X(t,x) ((t > 3*(x)/4) && (t < 5*(x)/4))
#define IS_0(t) IS_X(t,TimeZero)
#define IS_1(t) IS_X(t,TimeOne)
#define IS_S(t) IS_X(t,TimeStart)
#define IS_O(t) IS_X(t,TimeOff)

enum
{
  ISR_IDLE,   // nothing is/was happening (quiet)
  ISR_START,  // start of sequence, was waiting for a header signal
  ISR_BIT_ON, // transsmitting a bit (IR carrier turned on)
  ISR_BIT_OFF // in an OFF bit slot (IR carrier turned off)
}
isrState = ISR_IDLE;

unsigned long isrLastTimeStamp;
unsigned long isrRcvCmd;
unsigned long isrNewCmd;
byte isrBitLen = 22;
byte isrBitCnt;


void setup() {
  Serial.begin(9600);
  Serial.println("Controller ready");
}


void loop()
{
  long msg = irRecv();
  if (msg != 0) Serial.println(msg);
}

ISR( PCINT2_vect ) {
  // PIN_IR == #2 --> PD2;
  // receiving a modulated IR signal makes the pin go low (active low)
  byte transmitting = (PIND & (1<<2)) == 0;
  
  // compute elapsed time since last change
  unsigned elapsed;
  {
   unsigned long timeStamp = micros();
   elapsed = elapsedSince(isrLastTimeStamp,timeStamp);
   isrLastTimeStamp = timeStamp;
  }
  switch( isrState )
  {
    case ISR_IDLE :
      if( transmitting ) isrState = ISR_START;
      break;
    case ISR_START:
      isrBitCnt = 0;
      isrNewCmd = 0;
      isrBitLen = 22;
      if( !transmitting && IS_S(elapsed) )
        isrState = ISR_BIT_ON; // bits are now rolling
      else
        isrState = ISR_IDLE;   // wrong timing of start or pin state
      break;
    case ISR_BIT_ON:
      if( transmitting )
      {
        isrState = ISR_BIT_OFF;
        isrNewCmd <<= 1;
        isrBitCnt ++;
        if( IS_1(elapsed) )
        {
          isrNewCmd |= 1;
        }
        else if( IS_0(elapsed) )
        {
          // isrNewCmd |= 0;
        }
        else
          isrState = ISR_START; // bad timing, start over
        if( isrBitCnt == 7 ) // we have received 6 bit header (now expecting 7th bit)
        {
          isrBitLen = (isrNewCmd & (3<<3)) == 0 ? 30 : 22; // 2 vs 3 byte commands
        }
      }
      else isrState = ISR_IDLE; // bad state (should never get here...)
      break;
    case ISR_BIT_OFF:
      if( !transmitting && IS_O(elapsed) )
      {
        if( isrBitCnt == isrBitLen ) // is this the end?
        {
          isrState = ISR_IDLE;
          isrRcvCmd = isrNewCmd;
        }
        else      
          isrState = ISR_BIT_ON; // keep bits rolling
      }
      else
        if( IS_S(elapsed) )
          isrState = ISR_START;
        else
          isrState = ISR_IDLE;
      break;
  }
}


long irRecv()
{
 byte oldSREG = SREG;
 cli();
 long cmd = isrRcvCmd;
 isrRcvCmd = 0;
 SREG = oldSREG;
 return cmd;
}


