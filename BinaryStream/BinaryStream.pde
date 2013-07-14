

int risingEdge = 0;
int clockIn = 0;
int dataIn = 0;



void setup()
{
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  
  Serial.begin(9600);  
}

void loop()
{
  clockIn = digitalRead(6);
  dataIn = digitalRead(7);
  
  if  (!risingEdge && clockIn)
  {
    // Seen a rising edge on the clock
    risingEdge = 1;
    if (dataIn)
    {
      Serial.print("1");
    } else {
      Serial.print("0");
    }   
  }
  if (!clockIn)
  {
    risingEdge = 0;
  }
  
  
}
