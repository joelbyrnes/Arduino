int pirPin = 3;
int ledPin = 9;

const int numSamples = 10;
int readings[numSamples];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total

float v;

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10; 

void setup(){
  pinMode (pirPin,INPUT);
  pinMode (ledPin, OUTPUT);
  // what does this do? how do i set internal resistor?
  digitalWrite(pirPin, LOW);
  
  Serial.begin(115200);
  
    //give the sensor some time to calibrate
  Serial.print("-- Calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  
  // initialise array to avoid wacky values
  for (int i=0; i < numSamples; i++) {
    averaged(analogRead(pirPin));
  }
  
  Serial.println("starting");
}

void loop (){
  delay(10); 
  
  int readVal = analogRead(pirPin);    
  Serial.print(readVal);
  Serial.print(" ");
  
  int val=averaged(readVal);   
  v = ((float)val)/1024.0 * 5;
  Serial.print("Smoothed value: ");
  Serial.print(val);
  Serial.print(" Voltage: ");
  Serial.println(v);
  
  analogWrite(ledPin, val);
}

int averaged(int value) {
  // subtract the last reading:
  total= total - readings[index];         
  // add value 
  readings[index] = value; 

  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numSamples)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  return total / numSamples;
}
