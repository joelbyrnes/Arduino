
#define BUFSIZE 100

void setup()  
{
  Serial.begin(115200);
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
    if (strcmp(tok, "echo") == 0) {
      Serial.print("reply: ");
      if (value != NULL) {
        Serial.println(value);
      }
    } else {
      Serial.println("what?");
    }
    tok = strtok(NULL, " \t");
  }
}

