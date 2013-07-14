
void mainSetup() {
  setupDebug();
  debugln("Software serial - debugging output");
  
  setupComms();
  debugln("Comms set up.");
  setupControlPins();
  debugln("External hardware set up.");
  
#if defined(CONTROL_MODE_HBRIDGE)
  debugln("Using H-bridge mode.");
#endif
#if defined(CONTROL_MODE_SERVO)
  debugln("Using Servo mode.");
#endif
  
  debugln("Controller ready.");

  payload.reset();
  payload.println("Controller ready.");
  sendPayload();
  
// turn this on if it spams when turned on:  lastUpdateTime = millis();
}

void setupDebug() {  
  pinMode(PIN_DEBUG_TX, OUTPUT);
  pinMode(PIN_DEBUG_RX, INPUT);
  debugSerial.begin(9600);
  delay(100);
}

void setupComms() {
  Serial.begin(115200);

#if TARGETING_ENABLED
  while (!uart.connected()) {
    debugln("Connecting to external UART.");
    if (!connectUart()) {
      delay(500);
    }
  }
  
  // generate modulation for IR comms
  generate38khzOnPins9And10();
  
#endif
}

int connectUart() {
  // Test SPI communication
  if(uart.begin(0x90, 2400))  {
    debugln("SC16IS UART Bridge initialized successfully!"); 
    return false;
  } else {
    debugln("Could not initialise I2C SC16IS UART Bridge."); 
    return true;
  }
}

long updateDelayTotal = 0;

void mainLoop() {
  processSerial();
  checkUpdateDelay();
  checkTarget();
  beacon();
  sendStatus();
}  

void checkUpdateDelay() {
  long timeSinceLastUpdate = millis() - lastUpdateTime;
  if (uplinked && timeSinceLastUpdate > UPDATE_DELAY_MAX) {
    stopCar();
    centerSteering();
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

    /*    
    payload.print("droppedPackets: ");
    payload.print(droppedPackets);
    payload.print("\t");
    payload.print("sendTimeoutCount: ");
    payload.print(sendTimeoutCount);
    payload.print("\t");
    */
    payload.println();

    sendPayload();
    lastVRead = now;
  }
}

int n;

void processSerial() {
  // to swap between serials just switch out uart for Serial
  
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      debugln("overflow! discarding command. ");
      n = 0;
      break;
    }
    char in = Serial.read();
    if (in != '\n') {
      input[n++] = in;
    } else {
      input[n] = '\0'; // end string
      processData(input);
      n = 0;
    }
  }
}

void processData(char input[]) {
  debug("received data: ");
  debugln(input);
  
  // this assumes we received an intact line not prepended with crap. may miss input. 
  // also it would be more efficient to test this in the serial read part 
  // instead of reading a whole line of bad data. 
  if (input[0] != ':' || input[1] != ')') {
    if (input[0] == '/' && input[1] == '>') {
      debugln("** seeing console, restarting serproxy");
      resetSerproxy();
    }
    // check for serproxy exit and restart it
    if (input[0] == 's' && input[1] == 'e' && input[2] == 'r') {
      if (strstr(input, "server(1) exiting") != 0) {
        debugln("** received server(1) exiting from serproxy...!");
        resetSerproxy();
      }
    }
    debugln("invalid input.");
    return;
  }
  
  // skip prefix part of ":) drive 120"
  processCommand(&input[3]); 
}
  
void processCommand(char data[]) {
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
      // entering uplinked mode; expect regular command updates.
      startCommandMode();
      lastUpdateTime = millis();
      throttle(value);
    } else if (strcmp(tok, "steer") == 0) {
      // entering uplinked mode; expect regular command updates.
      startCommandMode();
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
      debug(value);
      debugln("");
    }
    tok = strtok(NULL, " \t");
  }
}

void resetSerproxy() {
  debugln("** resetting serproxy!");
  // send ctrl-c, enter, wait, /home/bin/serproxy, enter
  payload.write(0x03);
  payload.write('\n');
  sendPayload();
  debugln("** sent exit to serproxy.");
  
  delay(2500);
  payload.print("/home/bin/serproxy\n");
  sendPayload();

  debugln("** sent start to serproxy.");
}

void startCommandMode() {
  if (uplinked == false) {
    debugln("Entering command mode.");
    uplinked = true;
  }
}

void reply(char msg[]) {
  payload.print("reply: ");
  debug("echoing. ");
  if (msg != NULL) {
    debugln(msg);
    payload.println(msg);
    sendPayload();
  }
  //debugln("echoed.");
}

void debug(char *msg) {
#if DEBUG_ENABLED
  if (msg == NULL) return;
//  debugSerial.print(msg);
  Serial.print(msg);
#endif
}
void debugln(char *msg) {
#if DEBUG_ENABLED
  if (msg == NULL) return;
//  debugSerial.println(msg);
  Serial.println(msg);
#endif
}

void sendPayload() {
  payload.write(NULL);
  Serial.print((char *) payload.buffer());
  payload.reset();
}

