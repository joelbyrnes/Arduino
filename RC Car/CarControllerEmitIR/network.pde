
/* functions which send and receive data via RF12 */

byte sendPayloadIfYouCan(byte dest) {
  recvAndThrowAway();
  if (rf12_canSend()) {
    _sendPayload(dest);
    return 0;
  } else {
    debugln("can't send :(");
    resetPayload();
    return 1;
  }
}

byte sendPayloadWithinTime(byte dest, long maxTime) {
  long startTime = millis();
  
  while ((millis() - startTime) < maxTime) {
    recvAndThrowAway();
    if (rf12_canSend()) {
      _sendPayload(dest);
      return 0;
    }
  }
  resetPayload();
  debugln("WARNING could not send within time.");
  sendTimeoutCount++;
  return 1;
}

void mustSendPayload(byte dest) {
  recvAndThrowAway();
  while (!rf12_canSend()) {
    debugln("-X- can't send :( waiting...");
    //delay(2);
    recvAndThrowAway();
  }
  
  _sendPayload(dest);
}

void recvAndThrowAway() {
  if (rf12_recvDone() && rf12_crc == 0) {
    debugln("-X- OH GOD I RECEIVED AND IM IGNORING IT");
    droppedPackets++;
    lastUpdateTime = millis();
  }
}

// must have called canSend before this
void _sendPayload(byte dest) {
  // ensure payload can print correctly
  payload.print('\0');
  _sendMsg(dest, (byte *) payload.buffer(), payload.length());
  resetPayload();
}

void resetPayload() {
  char idAsChar = thisId + 65;
  payload.reset();
  payload.print(idAsChar);
  payload.print('|');
}

// must have called canSend before this
void _sendMsg(byte dest, byte *msg, byte sendLen) {
  
  //debug(" -> ");
  //debug("node ");
  //debug(char(dest + 65));  
  //debug(", ");
  //debug((int) sendLen);
  debug("b : '");
  debug((char *) msg);
  debugln("'");
  
  
  byte header;
  if (dest)
    header |= RF12_HDR_DST | dest;
  rf12_sendStart(header, msg, sendLen);
  // Wait for completion of rf12_sendStart() call, with specified low-power mode.
  //rf12_sendWait(1);
}


