
int redPin = 2;
int grnPin = 3;
int bluPin = 4;

//int waitTime = 200;

void setup()                   
{
  pinMode(redPin, OUTPUT); 
  pinMode(grnPin, OUTPUT); 
  pinMode(bluPin, OUTPUT);  
}

void loop()       
{
  int waitTime = 0;
  rgb(1, 0, 0, waitTime);
  rgb(1, 0, 0, waitTime);
  rgb(0, 1, 0, waitTime);
  rgb(0, 0, 1, waitTime);
  rgb(1, 0, 0, waitTime);
  rgb(0, 0, 1, waitTime);
  rgb(0, 0, 1, waitTime);
  rgb(0, 0, 1, waitTime);
  rgb(1, 0, 0, waitTime);
  rgb(0, 0, 1, waitTime);
  rgb(0, 0, 1, waitTime);
}

void rgb(int redOn, int grnOn, int bluOn, int waitTime) {
  digitalWrite(redPin, redOn);  
  digitalWrite(grnPin, grnOn);  
  digitalWrite(bluPin, bluOn);  
  if (waitTime > 0) {
    delay(waitTime);    
  }
}
