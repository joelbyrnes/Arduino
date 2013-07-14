/*
 * WiiChuckReader
 *
 * originally WiiChuckDemo, 2008 Tod E. Kurt, http://thingm.com/
 *
 */

#include <Wire.h>
#include "nunchuck_funcs.h"

int loop_cnt=0;

byte accx,accy,accz,zbut,cbut;
int ledPin = 13;


void setup()
{
    Serial.begin(19200);
    nunchuck_setpowerpins();
    nunchuck_init(); // send the initilization handshake
    
    Serial.print("WiiChuckDemo ready\n");
}

void loop()
{
    nunchuck_get_data();

    accx  = nunchuck_accelx(); // ranges from approx 70 - 182
    accy  = nunchuck_accely(); // ranges from approx 65 - 173
    accz  = nunchuck_accelz();
    zbut = nunchuck_zbutton();
    cbut = nunchuck_cbutton(); 
            
    Serial.print("accx: "); Serial.print((byte)accx,DEC);
    Serial.print("\taccy: "); Serial.print((byte)accy,DEC);
    Serial.print("\taccz: "); Serial.print((byte)accz,DEC);
    Serial.print("\tzbut: "); Serial.print((byte)zbut,DEC);
    Serial.print("\tcbut: "); Serial.println((byte)cbut,DEC);

    delay(50);
}

