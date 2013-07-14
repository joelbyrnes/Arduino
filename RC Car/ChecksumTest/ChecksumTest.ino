void setup() {
  Serial.begin(57600);
  Serial.println("test");
}
  
void loop() {
  delay(500);
  test1();
}

void test2() {
  char* str = "Cz";
  Serial.println(str);
//  byte* str = (byte*) str;
  Serial.print("testing ");
  Serial.println((char*) str);
  
  byte cs = generateChecksum((byte*) str, 2);
  Serial.println(cs);
  
  byte* csed = getChecksummedValue((byte*) str, 2);
  Serial.println((char*) csed);
}

void test1() {
    
  byte* str = (byte*) "Cz";

  byte* bytes = getChecksummedValue(str, 2);
  Serial.print("bytes ");
  Serial.println((char*) bytes);
  Serial.println(bytes[2], DEC);

  if (validateChecksum(bytes, 3)) {
    Serial.println("valid.");
  } else {
    Serial.println("invalid.");
  }
  Serial.print("message is: ");
  Serial.println((char*) bytes);
  
  Serial.println();
}

byte _cksummed[6];

// add the checksum at the end. returns a new array without modifying original.
byte* getChecksummedValue(byte bytes[], int length) {
  Serial.print("input ");
  Serial.print((char*) bytes);
  for (int i = 0; i < length; i++) {
    _cksummed[i] = bytes[i];
  }
  _cksummed[length] = generateChecksum(bytes, length);
  _cksummed[length + 1] = '\0';
  Serial.print(", after copy ");
  Serial.println((char*) _cksummed);
  return _cksummed;
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

