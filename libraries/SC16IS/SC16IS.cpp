/*
  SC16IS library for using UART features.
  Copyright (c) 2011 Joel Byrnes.  All right reserved.

  Based on code found at http://www.geocities.jp/dotallcafe/arduino/SC16IS750_i2c.txt
  Which is similar to parts of the Wifly library using the same chip.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "SC16IS.h"

#include <Wire.h>

// SC16IS750 Register Definitions
#define RHR        0x00 << 3
#define THR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TCR        0x06 << 3 // same as MSR?
#define TLR        0x07 << 3 // same as SPR?
#define TXLVL      0x08 << 3
#define RXLVL      0x09 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
// reserved        0x0D << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

// wtf? this was defined in the original library but is not in the data sheet
#define DLAB       0x80 << 3

// special register set
#define DLL        0x00 << 3
#define DLH        0x01 << 3

// enhanced register set
#define EFR        0x02 << 3
#define XON1       0x04 << 3  
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3 // TCR in the datasheet
#define XOFF2      0x07 << 3 // TLR in the datasheet

// are these in global scope or are they class members?
uint8_t inBuf[SC16IS_BUFSIZE];

// this only exists because of some stupid ambiguity on the type of the defined value THR
uint8_t _SC16IS_THR = THR;

// Constructors ////////////////////////////////////////////////////////////////

SC16IS::SC16IS() {
}

// Private Methods //////////////////////////////////////////////////////////////

uint8_t SC16IS::I2C_ReadByte(uint8_t reg) {
  Wire.beginTransmission(i2cadr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cadr, (uint8_t) 1);  
  uint8_t data = Wire.read();  
  return data;
}

void SC16IS::I2C_WriteByte(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(i2cadr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// TODO limit to standard baud rates?
void SC16IS::setBaud(long baud) {
	/*
	 MCR[7] is either (0) divide-by-1 or (1) divide-by-4 prescaler
	 divisor = (XTAL1 / prescaler) / ( desired baud rate * 16 )
	 ie if you want 115200 and MCR[7] is set to 0:
	 divisor = 14745600 / ( 115200 * 16 )
	  = 8 == 0x08
	 ie DL HIGH = 0x00 and DL LOW = 0x08
	
	 0x30=19.2K, 0x08 =115.2K with Xtal = X1=14.7456 MHz
	 */
  // not using MCR[7] setting here

  long divisor = 14745600 / (baud * 16);
  uint8_t dll = divisor & 0x00FF;
  // get top byte and shift down 
  uint8_t dlh = (divisor & 0xFF00) >> 8;

  I2C_WriteByte(LCR, 0x80);  // 0x80 to program baudrate
  I2C_WriteByte(DLL, dll);
  I2C_WriteByte(DLH, dlh); 
}

int SC16IS::requestBytes(uint8_t n) {
  Wire.beginTransmission(i2cadr);
  Wire.write(_SC16IS_THR);
  Wire.endTransmission();
  Wire.requestFrom(i2cadr, n);
  return Wire.available();
}


// Public Methods //////////////////////////////////////////////////////////////

// Initialize SC16IS750
uint8_t SC16IS::begin(long hexAddress, long baud) {
  	i2cadr = (hexAddress >> 1);

  	Wire.begin();

	setBaud(baud);

	I2C_WriteByte(LCR, 0xBF); // access EFR register
	I2C_WriteByte(EFR, 0x10); // enable enhanced registers
	I2C_WriteByte(LCR, 0x03); // 8 data bit, 1 stop bit, no parity

	// can't these be done in the same call?
	I2C_WriteByte(FCR, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
	I2C_WriteByte(FCR, 0x01); // enable FIFO mode

	// Perform read/write test to check if UART is working
	I2C_WriteByte(SPR,'H');
	char data = I2C_ReadByte(SPR);
	// change it to something else for next time we check
	I2C_WriteByte(SPR,'x');

	if(data == 'H') {
		connStatus = 1;
  	}
	return connStatus;
}

void SC16IS::end() {
	connStatus = 0;
}

void SC16IS::enableIrDA(uint8_t enable) {
	// turn on 7th bit ie 64 or 0x40 to enable IrDA SIR. ONLY writable if EFR[4] (0x10) is enabled. 
	// TODO this should really read the current value, toggle the bit and write that. 
  	I2C_WriteByte(MCR, enable? 0x40 : 0x00);
}

// I don't know why you would, but for completeness.. disables FIFOs, IrDA, interrupts, etc
void SC16IS::disableEnhancements(uint8_t disable) {
	// TODO this should really read the current value, toggle the bit and write that. 
	I2C_WriteByte(EFR, disable? 0x00 : 0x10); // enable enhanced registers
}

int SC16IS::available(void) {
	// Recv FIFO level register RXLVL - bit 7 not used, max 64 (0x40). 
  	return I2C_ReadByte(RXLVL);
}

int SC16IS::txBufferAvailable(void) {
	// Transmit FIFO level register TXLVL - bit 7 not used, max 64 (0x40). 
  	return I2C_ReadByte(TXLVL);
}

int SC16IS::read(void) {
	return I2C_ReadByte(RHR);
}

uint8_t SC16IS::connected() {
	// TODO we could do an active check here rather than just use the stored value.
	return connStatus;
}

uint8_t* SC16IS::readBytes(uint8_t n) {
  // will not attempt to get more bytes at a time than the buffer can hold
  if (n > SC16IS_BUFSIZE) 
    n = SC16IS_BUFSIZE;

  int get = requestBytes(n);

  int i = 0;
  while (i < get && Wire.available()) {
    inBuf[i++] = Wire.read();
  }

  return inBuf;
}

/* According to the PDF, there are TX and RX trigger levels of 8, 16, 32 and 56 "spaces".
 * So writing might not actually send until there are 8 characters?
 */

size_t SC16IS::write(uint8_t b) {
	I2C_WriteByte(_SC16IS_THR, b);
}

void SC16IS::write(const char *buffer, uint8_t size) {
	writeBytes((uint8_t*) buffer, size);

	// stolen from Print.cpp - but doing a write for every byte with Wire would be inefficient. 
  	// uint8_t n = 0;
  	// while (size--) {
  	//   n += write(*buffer++);
  	// }
  	// return n;
}

// sends max 32 bytes at a time, although the buffer should be 64. any more get lost. or maybe they are lost in the receive?
void SC16IS::writeBytes(uint8_t* bytes, uint8_t n) {
  // TODO this should maybe check the size of the TX FIFO, and/or limit the max size
  Wire.beginTransmission(i2cadr);
  Wire.write(_SC16IS_THR);
  for (int i = 0; i < n; i++) {
    Wire.write(bytes[i]);
  }
  Wire.endTransmission();
}

// flush the RX FIFO
void SC16IS::flush() {
	// TODO: "Serial.flush() waits for outgoing data to transmit before clearing the buffer contents."
	I2C_WriteByte(FCR, 0x03); // reset RXFIFO, enable FIFO mode
	// since we only want to clear RX we don't want to turn off FIFO mode even temporarily. I hope that works. 
}

// I don't think the device supports peeking, without having to buffer in software. 
// int SC16IS::peek() {
// 	return 0;
// }

