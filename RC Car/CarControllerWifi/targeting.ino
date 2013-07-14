
void checkTarget() {
#if TARGETING_ENABLED

  int avail = uart.available();
  if (avail > 1) {
    // TODO max avail size check?
    
    // the buffer returned is max 64 bytes. that should be heaps for target data. 
    byte* data = uart.readBytes(avail);
    // we assume we did actually get avail bytes, no more. 
    // this is a bit redundant since the data may validly contain 0. 
    data[avail] = '\0';
    targetData(data, avail);
  }

#endif
}

byte msgBuf[MSGBUFSIZE];
int msgbufptr = 0;

void targetData(byte* data, int datasize) {
  //aprintf("processing datasize %d, '%s'", datasize, data);
	int i = 0;
        byte in;
	while (i < datasize) {
		if (msgbufptr >= MSGBUFSIZE) {
			debugln("target buffer overflow!");
			msgbufptr = 0;
		}
		in = data[i++];
		if (in != '\n') {
			msgBuf[msgbufptr++] = in;
		} else {
			msgBuf[msgbufptr] = '\0'; // end string
			potentialTarget(msgBuf, msgbufptr);
			msgbufptr = 0;
		}
	}
}

void clinton(byte* frag, int datasize) {
	while (frag < (frag + datasize) && *frag) {
		if (msgbufptr >= BUFSIZE) {
		  debugln("target buffer full!");
                  msgbufptr = 0;
		}
	
		if (*frag == '\n') {
			msgBuf[msgbufptr] = '\0';
			potentialTarget(msgBuf, msgbufptr);
			msgbufptr = 0;
		} else {
			msgBuf[msgbufptr++] =*frag;
		}
		frag++;
	}
}

void potentialTarget(byte* msg, int len) {
//  aprintf("target? len %d, '%s'\n", len, (char*) msg);
  if (validateChecksum(msg, len)) {
    // adding offset of 1 because sending doesn't work unless the msg changes at front??? added a looping byte at front. 
    char* charmsg = (char*) msg;
    targetSeen(charmsg + 1);
  } else {
    //aprintf("invalid target: %s", (char*) msg);
  }
}

void targetSeen(char* target) {
  // if we are shooting and have a target, we hit them!
  if (shootValue > 0) {
    aprintf("st: %s", target);
  } else {
    // otherwise just report their presence. 
    aprintf("tt: %s", target);    
  }
}

unsigned long lastBeacon = 0;
char* beaconMsg = "1A";

// send message over uart, keep the buffer full so it sends constantly. 
void beacon() {
  // every 60ms is plenty to keep the 64 byte buffer full without doing Wire calls every loop. 
  if (millis() - lastBeacon < 60) return;
  
  int avail = uart.txBufferAvailable();
//  dprintf("refill beacon buffer");

  while (avail > 4) {
    // increment a prefix byte because otherwise the sending or receiving stops after a bit???
    byte foo = beaconMsg[0];
    if (++foo > 254) foo = 1;
    beaconMsg[0] = foo;
 
    sendChecksummed((byte*) beaconMsg, 2);
    avail -= 4;
    lastBeacon = millis();
  }
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

void sendChecksummed(byte* bytes, int len) {
  beaconOutput.print((char*) bytes);
  beaconOutput.write(generateChecksum((byte*) beaconOutput.buffer(), len));
  beaconOutput.print("\n");
  beaconOutput.write(NULL);
  
  uart.write((char *) beaconOutput.buffer());
  beaconOutput.reset();
}

