
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
	#define serialPrintByte(b) Serial.write(b)
#else
	#include "WProgram.h"
	#define serialPrintByte(b) Serial.print(b,BYTE)
#endif

#include "bitlash.h"
#include <Wire.h>
#include <SC16IS.h>
 
SC16IS uart;
boolean autoReceive = true;

numvar SC16IS_func(void) {
  if(uart.begin(0x90, getarg(1)))  {
    Serial.println("Connected!"); 
    return 0;
  } else {
    Serial.println("Could not connect over i2c."); 
    return 1;
  }
}

// what's bitlash type for boolean?
numvar irda_func(void) {
  uart.enableIrDA(getarg(1));
  return 0;
}

// according to the PDF, there are TX and RX trigger levels of 8, 16, 32 and 56 "spaces".
// test if it really doesn't transmit until there are 8 characters. 

// what's bitlash type for string? does it keep the newline?
numvar sendMsg_func(void) {
  char* msg = (char*) getarg(1);
  uart.write(msg, strlen(msg));
  return 0;
}

numvar sendCksum_func(void) {
  // TODO how to getarg() char* as well as numbers??
  char* msg = (char *) getarg(1);
//  char* msg = "foo";
  int checksum = getCheckSum((char *) msg);
  Serial.print("checksum: ");
  Serial.println(checksum, HEX);
  // TODO use checksum, check value at other end
  uart.write(msg, strlen(msg));
  uart.write(checksum);
  return 0;
}

numvar autoRecv_func(void) {
  autoReceive = getarg(1);
  return 0;
}

numvar recv_func(void) {
  if (!uart.connected()) return 0;
  int avail = uart.available();
  if (avail > 0) {
    char* data = (char*) uart.readBytes(avail);
    data[avail] = NULL; // finalise string if it wasn't already
    Serial.print(data);
  }
  return 0;
}
  
// TODO what can you do with return variables in bitlash?

void setup(void) {
	initBitlash(57600);		// must be first to initialize serial port

	addBitlashFunction("SC16IS", (bitlash_function) SC16IS_func);
	addBitlashFunction("sendMsg", (bitlash_function) sendMsg_func);
//	addBitlashFunction("sendCksum", (bitlash_function) sendCksum_func);
	addBitlashFunction("irda", (bitlash_function) irda_func);
	addBitlashFunction("autoRecv", (bitlash_function) autoRecv_func);
	addBitlashFunction("recv", (bitlash_function) recv_func);

	addBitlashFunction("tone", (bitlash_function) func_tone);
	addBitlashFunction("notone", (bitlash_function) func_notone);
}

void loop(void) {
  runBitlash();
  
  if (autoReceive) {
    recv_func();
  }
}

numvar func_tone(void) {
//	if (getarg(0) == 2) 
tone(getarg(1), getarg(2));
//	else tone(getarg(1), getarg(2), getarg(3));
	return 0;
}

numvar func_notone(void) {
	noTone(getarg(1));
	return 0;
}


