/*
  SC16IS.h - library for using UART features.
  Copyright (c) 2011 Joel Byrnes.  All right reserved.

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

#ifndef SC16IS_h
#define SC16IS_h

#define SC16IS_BUFSIZE 64

#include <inttypes.h>

#include "Print.h"
#include "Stream.h"

class SC16IS : public Print 
{
  private:
	uint8_t i2cadr;
	uint8_t connStatus;

	uint8_t I2C_ReadByte(uint8_t);
	void I2C_WriteByte(uint8_t, uint8_t);
	void setBaud(long baud);
	int requestBytes(uint8_t n);

  public:
    SC16IS();

    uint8_t begin(long address, long baud);
    void end();
	uint8_t connected();
	
	void enableIrDA(uint8_t enable);
	void disableEnhancements(uint8_t enable);
	
    int txBufferAvailable();
    int available();
    int read();
	uint8_t* readBytes(uint8_t n);
	
	/* According to the PDF, there are TX and RX trigger levels of 8, 16, 32 and 56 "spaces".
	 * So writing might not actually send until there are 8 characters?
	 */
	size_t write(uint8_t byte);
	void write(const char *buffer, uint8_t size);
	void writeBytes(uint8_t *buffer, uint8_t size);
	
    void flush();
	// not implementing Stream because the device doesn't support peek, without having to do something strange. 
	// int peek();

	// what does this actually do? do we need it? if left out, it compiles ok. 
    using Print::write; // pull in write(str) and write(buf, size) from Print
};

#endif
