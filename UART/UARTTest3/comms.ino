
void generate38khz() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Clear Timer on Compare Match (CTC) Mode
  bitWrite(TCCR1A, WGM10, 0);
  bitWrite(TCCR1A, WGM11, 0);
  bitWrite(TCCR1B, WGM12, 1);
  bitWrite(TCCR1B, WGM13, 0);

  // Toggle OC1A and OC1B on Compare Match.
  bitWrite(TCCR1A, COM1A0, 1);
  bitWrite(TCCR1A, COM1A1, 0);
  bitWrite(TCCR1A, COM1B0, 1);
  bitWrite(TCCR1A, COM1B1, 0);

  // No prescaling
  bitWrite(TCCR1B, CS10, 1);
  bitWrite(TCCR1B, CS11, 0);
  bitWrite(TCCR1B, CS12, 0);

  OCR1A = 210;
  OCR1B = 210;

  Serial.println("Generating 38kHz signal on pins 9 & 10");
}

// add the checksum at the end. returns a new array without modifying original.
byte* getChecksummedValue(byte bytes[], int length) {
  Serial.print(" input ");
  Serial.print((char*) bytes);
  byte newbytes[length + 1];
  for (int i = 0; i < length; i++) {
    newbytes[i] = bytes[i];
  }
  newbytes[length] = generateChecksum(bytes, length);
  Serial.print(" with cs ");
  Serial.println((char*) newbytes);
  Serial.println((long int) newbytes, DEC);
  return newbytes;
}

// Validates the 1-byte checksum on a string where the checksum is at the end. length includes checksum.
// Returns 1 on valid checksum, 0 otherwise
// also changes the value, so it can be used immediately once validated.
int validateChecksum(byte input[], int length) {
	byte cksum = input[length - 1];
	input[length - 1] = '\0';
	// Check that the checksums match up
	return cksum == generateChecksum(input, length - 1) ? 1 : 0;
}

// Calculates the checksum for a given byte array of given length.
byte generateChecksum(byte buffer[], int length) {
	int i;
	byte XOR;
	byte c;
	// Calculate checksum
	for (XOR = 0, i = 0; i < length; i++) {
		c = buffer[i];
		if (c == '\0') break;
		XOR ^= c;
	}
	return XOR;
}
