
#define BUFSIZE 100

// TODO put this in a separate file
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }

    virtual void write(uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    
private:
    byte fill, buf[BUFSIZE];
};

PacketBuffer payload;   // sending buffer

void setup() {
 
  Serial.begin(57600);
  debugln("Controller ready");
}

void loop() {
  processData(Serial.read());    
}

void processData(char data[]) {
  char *tok = NULL;
  char *value = NULL;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "echo") == 0) {
      reply(value);
    } else {
      debug("what? : ");
      debug(tok);
      debug(" ");
      debugln(value);
    }
    tok = strtok(NULL, " \t");
  }
}

void reply(char msg[]) {
  payload.print("reply: ");
  debugln("echoing.");
  if (msg != NULL) {
    payload.println(msg);
    Serial.print((char*) payload.buffer());
  }
}

void debug(char *msg) {
  Serial.print(msg);
}
void debugln(char *msg) {
  Serial.println(msg);
}

