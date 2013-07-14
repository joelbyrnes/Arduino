
#define BUFSIZE 100

void setup()  
{
  Serial.begin(19200);
  Serial.println("Hello, world?");
}

void loop() {
  serialEcho();
}  

char input[BUFSIZE];
int n;

void serialEcho() {
  while (Serial.available()) {
    if (n >= BUFSIZE) {
      Serial.println("overflow! discarding command. ");
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

void processData(char data[]) {
  char *tok;
  char *value;
  
  tok = strtok (data," \t");
  while (tok != NULL) {
    value = strtok(NULL, " \t");
    if (strcmp(tok, "ping") == 0) {
      reply(value);
    } else if (strcmp(tok, "pan") == 0) {
      pan(value);
    } else {
      Serial.println("what?");
    }
    tok = strtok(NULL, " \t");
  }
}

void reply(char msg[]) {
  Serial.print("pong: ");
  if (msg != NULL) {
    Serial.println(msg);
  }
}

void pan(char value[]) {
  Serial.print("pan: ");
  if (value != NULL) {
    int pan = int(value);
    Serial.println(pan);
    // set pan... 
  }
}
  


