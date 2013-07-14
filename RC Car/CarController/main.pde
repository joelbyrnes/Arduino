
void mainSetup() {
  setupControlPins();
  setupComms();
  
  emitName();

  debugln("Software serial - debugging output");
  debugln("Controller ready");

  payload.reset();
  payload.print("Controller ready with ID ");
  payload.println((int) thisId);
  mustSendPayload(NET_BROADCAST); // broadcast
  
#if defined(CONTROL_MODE_HBRIDGE)
  debugln("using H-bridge mode");
#endif
#if defined(CONTROL_MODE_SERVO)
  debugln("using Servo mode");
#endif
  
// turn this on if it spams when turned on:  lastUpdateTime = millis();
}

void emitName() {
  Serial.print("Node ");
  Serial.print(thisId, DEC);
  Serial.print(" (");
  Serial.print(char(thisId + 64));
  Serial.println(")");
}
  
void setupControlPins() {
#if defined(CONTROL_MODE_HBRIDGE)
  pinMode(PIN_FWD, OUTPUT);
  pinMode(PIN_REV, OUTPUT);
  pinMode(PIN_LEFT, OUTPUT);
  pinMode(PIN_RIGHT, OUTPUT);
  analogWrite(PIN_FWD, 0);
  analogWrite(PIN_REV, 0);
  digitalWrite(PIN_LEFT, LOW);
  digitalWrite(PIN_RIGHT, LOW);
#endif
#if defined(CONTROL_MODE_SERVO)
  throttleServo.attach(PIN_THROTTLE);
  steerServo.attach(PIN_STEER);
#endif
    
  pinMode(PIN_SHOOT, OUTPUT);
  digitalWrite(PIN_SHOOT, LOW);

  panServo.attach(PIN_PAN);
  tiltServo.attach(PIN_TILT);
  
  initHardware();
}

void setupComms() {
  Serial.begin(9600);

  pinMode(PIN_DEBUG_TX, OUTPUT);
  pinMode(PIN_DEBUG_RX, INPUT);
  debugSerial.begin(9600);

  thisId = rf12_config();
}

long updateDelayTotal = 0;

void mainLoop() {
  processPackets();
  
  long timeSinceLastUpdate = millis() - lastUpdateTime;
  if (uplinked && timeSinceLastUpdate > UPDATE_DELAY_MAX) {
    stopCar();
    // TODO turn on a LED to indicate loss of signal or updates
    debugln("WARNING update timeout!");
    
    // so that it won't every loop spam until there is an update
    lastUpdateTime = millis();

    updateDelayTotal += timeSinceLastUpdate;
    if (updateDelayTotal > 30000) {
      // parent node has gone away, stop waiting for it. 
      debugln("Giving up waiting for updates, resetting.");
      exitCommandMode();
      updateDelayTotal = 0;
    }
  }
 
  sendStatus();
  
  //emitName();
}  

void exitCommandMode() {
  uplinked = false;
  initHardware();
}

void sendStatus() {
  long now = millis();
  if ((now - lastVRead) > 5000) {
    long Vcc_mV = readVcc_mV();
    payload.print("Vcc: ");
    payload.print(Vcc_mV / 1000.0);
    payload.print("\t");
    float Vcar = getVoltage(Vcc_mV, PIN_V_CARBATT);
    payload.print("Vcar: ");
    payload.print(Vcar);
    payload.print("\t");
    
    payload.print("droppedPackets: ");
    payload.print(droppedPackets);
    payload.print("\t");
    payload.print("sendTimeoutCount: ");
    payload.print(sendTimeoutCount);
    payload.print("\t");
    payload.println();
        
    // spend up to 50ms trying to send status
   if (sendPayloadWithinTime(NODE_CONTROL, 50) == 0) { 
      debugln("sent status.");
    } else {
      debugln("sending status failed.");
    }
    lastVRead = now;
  }
}


void processPackets() {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_len > PACKET_LEN_MIN) {
    //Serial.print("<");
    //Serial.print((int) RF12_HDR_MASK & rf12_hdr);
    //Serial.print("> ");    
    
    // copy packet into buffer before it disappears
    for (byte i = 0; i < rf12_len; ++i) {
      if (rf12_data[i] != '\n') {
        input[i] = rf12_data[i];
      } else {
        input[i] = '\0'; // end string
      }
    }
    
    processData(input);    
    // clean up input
    //for (int j=0; j < BUFSIZE; j++) {
    //  input[j] = '\0';
    //}
  }
}

void processData(char data[]) {
  //debugln("received a packet.");
  
  char *tok = NULL;
  char *value = NULL;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "echo") == 0) {
      reply(value);
    } else if (strcmp(tok, "pan") == 0) {
      pan(value);
    } else if (strcmp(tok, "tilt") == 0) {
      tilt(value);
    } else if (strcmp(tok, "shoot") == 0) {
      shoot(value);
    } else if (strcmp(tok, "throttle") == 0) {
      lastUpdateTime = millis();
      throttle(value);
    } else if (strcmp(tok, "steer") == 0) {
      steer(value);
    } else if (strcmp(tok, "stop") == 0) {
      lastUpdateTime = millis();
      stopCar();
    } else if (strcmp(tok, "bye") == 0) {
      lastUpdateTime = millis();
      exitCommandMode();
    } else {
      debug("what? : ");
      debug(tok);
      debug(" ");
      debugln(value);
    }
    tok = strtok(NULL, " \t");
    
    // entering uplinked mode; expect regular command updates.
    startCommandMode();
  }
}

void startCommandMode() {
  if (uplinked == false) {
    debugln("Entering command mode.");
  }
  uplinked = true;
}

void reply(char msg[]) {
  payload.print("reply: ");
  debug("echoing. ");
  if (msg != NULL) {
    //debugln(msg);
    payload.println(msg);
    mustSendPayload(NODE_CONTROL);
  }
  //debugln("echoed.");
}

void debug(char *msg) {
#if DEBUG_ENABLED
  debugSerial.print(msg);
  //Serial.print(msg);
#endif
}
void debugln(char *msg) {
#if DEBUG_ENABLED
  debugSerial.println(msg);
  //Serial.println(msg);
#endif
}

