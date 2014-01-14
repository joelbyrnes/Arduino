
#define HUE_PIN A3   // PB3 / pin 2
#define LIGHT_PIN A1 // PB2 / pin 7 

int redPin = 4; // PB4 / pin 3
int grnPin = 0; // PB0 / pin 5 
int bluPin = 1; // PB1 / pin 6

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numSamples = 32;

int h_readings[numSamples];      // the readings from the analog input
int h_index = 0;                  // the index of the current reading
int h_total = 0;                  // the running total
int l_readings[numSamples];      // the readings from the analog input
int l_index = 0;                  // the index of the current reading
int l_total = 0;                  // the running total

void setup()                    // run once, when the sketch starts
{
  pinMode(redPin, OUTPUT);  
  pinMode(grnPin, OUTPUT);  
  pinMode(bluPin, OUTPUT);  

  pinMode(HUE_PIN, INPUT);  
  pinMode(LIGHT_PIN, INPUT);  
  
//  Serial.begin(115200);          
  
  // initialise array to avoid wacky values
  for (int i=0; i < numSamples; i++) {
    averagedHue();
    averagedLight();
  }
}

double h=0.0, s=1.0, l=0.5;

void loop() {
  int avgHue = averagedHue();    // Read the pin and display the value
  int avgLight = averagedLight();    // Read the pin and display the value
//  Serial.println(avgHue);
//  Serial.println(avgLight);

  h = 360. * ((double) avgHue)/1024.;
  l = ((double) avgLight)/1024.;
  
//  Serial.print(h);
//  Serial.print(" ");
//  Serial.print(s);
//  Serial.print(" ");
//  Serial.println(l);
  setHSLColour(h, s, l);
}

int averagedHue() {
  // subtract the last reading:
  h_total = h_total - h_readings[h_index];         
  // read from the sensor:  
  h_readings[h_index] = analogRead(HUE_PIN); 
  // add the reading to the total:
  h_total = h_total + h_readings[h_index++];       

  // if we're at the end of the array, restart
  if (h_index >= numSamples) h_index = 0;                           

  // calculate the average:
  return h_total / numSamples;
}

int averagedLight() {
  // subtract the last reading:
  l_total = l_total - l_readings[l_index];         
  // read from the sensor:  
  l_readings[l_index] = analogRead(LIGHT_PIN); 
  // add the reading to the total:
  l_total = l_total + l_readings[l_index++];       

  // if we're at the end of the array, restart
  if (l_index >= numSamples) l_index = 0;                           

  // calculate the average:
  return l_total / numSamples;
}

// https://gist.github.com/xpansive/1337890
void setHSLColour(double hue, double sat, double light) {
  sat *= light <.5? light : 1-light;
 
  double v = light+sat;
  //setHSVColour(hue, 2*sat/v, v);
  setLedColorHSV(hue, 2*sat/v, v);
}

// pwm using arduino-tiny cores
void colour(byte red, byte grn, byte blu) {
  analogWrite(redPin, red);
  analogWrite(grnPin, grn);
  analogWrite(bluPin, blu);
}

// do 256 iterations, given different values for each of R G B until they are "used up"
void colourBitBang(int red, int grn, int blu) {
  for (int i = 0; i < 256; i++) {
    // if colour is greater than 0, turn it on, else off, then decrement.
    rgb((red-- > 0), (grn-- > 0), (blu-- > 0));
  }
}

void rgb(int redOn, int grnOn, int bluOn) {
  digitalWrite(redPin, redOn);  
  digitalWrite(grnPin, grnOn);  
  digitalWrite(bluPin, bluOn);  
}

/*
 * Created 1 January 2011
 * By Eduardo A. Flores Verduzco
 * http://eduardofv.com
 */
 
//Convert a given HSV (Hue Saturation Value) to RGB(Red Green Blue) and set the led to the color
//  h is hue value, integer between 0 and 360
//  s is saturation value, double between 0 and 1
//  v is value, double between 0 and 1
//http://splinter.com.au/blog/?p=29
void setLedColorHSV(double h, double s, double v) {
  //this is the algorithm to convert from RGB to HSV
  double r=0; 
  double g=0; 
  double b=0;

  double hf=h/60.0;

  int i=(int)floor(h/60.0);
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
  double tv = v * (1 - s * (1 - f));

  switch (i)
  {
  case 0: //rojo dominante
    r = v;
    g = tv;
    b = pv;
    break;
  case 1: //verde
    r = qv;
    g = v;
    b = pv;
    break;
  case 2: 
    r = pv;
    g = v;
    b = tv;
    break;
  case 3: //azul
    r = pv;
    g = qv;
    b = v;
    break;
  case 4:
    r = tv;
    g = pv;
    b = v;
    break;
  case 5: //rojo
    r = v;
    g = pv;
    b = qv;
    break;
  }

  //set each component to a integer value between 0 and 255
  byte red = constrain((int) 255 * r, 0, 255);
  byte green = constrain((int) 255 * g, 0, 255);
  int blue = constrain((int) 255 * b, 0, 255);

  colour(red,green,blue);
}

