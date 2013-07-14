
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

/*
// Validates the checksum on an (for instance NMEA) string
// Returns 1 on valid checksum, 0 otherwise
int validateChecksum(void) {
  char gotSum[2];
  gotSum[0] = buffer[strlen(buffer) - 2];
  gotSum[1] = buffer[strlen(buffer) - 1];
  // Check that the checksums match up
  if ((16 * atoh(gotSum[0])) + atoh(gotSum[1]) == getCheckSum(buffer)) return 1;
  else return 0;
}
*/

// Calculates the checksum for a given string
// returns as integer
int getCheckSum(char *string) {
  int i;
  int XOR;
  int c;
  // Calculate checksum
  for (XOR = 0, i = 0; i < strlen(string); i++) {
    c = (unsigned char)string[i];
    if (c == '\0') break;
    XOR ^= c;
  }
  return XOR;
}


