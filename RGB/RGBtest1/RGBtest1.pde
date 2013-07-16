
int redPin = 2;
int grnPin = 3;
int bluPin = 4;

int waitTime = 200;

void setup()                   
{
  pinMode(redPin, OUTPUT); 
  pinMode(grnPin, OUTPUT); 
  pinMode(bluPin, OUTPUT);  
}

void loop()       
{
  rgb(1, 0, 0);
  rgb(0, 1, 0);
  rgb(0, 0, 1);
  delay(waitTime);
  
  rgb(1, 0, 0);
  rgb(1, 1, 0);
  rgb(1, 0, 1);
  delay(waitTime);
  
  rgb(1, 1, 0);
  rgb(0, 1, 0);
  rgb(0, 1, 1);
  delay(waitTime);
  
  rgb(1, 0, 1);
  rgb(0, 1, 1);
  rgb(0, 0, 1);
  delay(waitTime);
  
  rgb(1, 1, 1);
  rgb(1, 1, 1);
  rgb(1, 1, 1);
  delay(waitTime);
}

void pulse(int pin) {
  digitalWrite(pin, HIGH);  
  delay(waitTime);                 
  digitalWrite(pin, LOW); 
  delay(waitTime);
}

void rgb(int redOn, int grnOn, int bluOn) {
  digitalWrite(redPin, redOn);  
  digitalWrite(grnPin, grnOn);  
  digitalWrite(bluPin, bluOn);  
  delay(waitTime);    
  digitalWrite(redPin, LOW);  
  digitalWrite(grnPin, LOW);  
  digitalWrite(bluPin, LOW);  
}
