
#define POT_PIN A0

int rpin = 9;
int gpin = 10;
int bpin = 11;
float h;
int h_int;
int r=0, g=0, b=0;

int val=0;


// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numSamples = 32;

int readings[numSamples];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total


//void h2rgb(float h, int &R, int &G, int &B);



void setup()                    // run once, when the sketch starts
{
  Serial.begin(115200);          
  
  // initialise array to avoid wacky values
  for (int i=0; i < numSamples; i++) {
    averagedRead();
  }
}


void loop()                     // run over and over again
{
  val=averagedRead();    // Read the pin and display the value
  //Serial.println(val);
  h = ((float)val)/1024;
  h_int = (int) 360*h;
  h2rgb(h,r,g,b);
  Serial.print("Smoothed value: ");
  Serial.print(val);
  Serial.print(" = Hue of ");
  Serial.print(h_int);
  Serial.print("degrees. RGB: ");
  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);

  analogWrite(rpin, r);
  analogWrite(gpin, g);
  analogWrite(bpin, b);
}

void h2rgb(float H, int& R, int& G, int& B) {

  int var_i;
  float S=1, V=1, var_1, var_2, var_3, var_h, var_r, var_g, var_b;

  if ( S == 0 )                       //HSV values = 0 ÷ 1
  {
    R = V * 255;
    G = V * 255;
    B = V * 255;
  }
  else
  {
    var_h = H * 6;
    if ( var_h == 6 ) var_h = 0;      //H must be < 1
    var_i = int( var_h ) ;            //Or ... var_i = floor( var_h )
    var_1 = V * ( 1 - S );
    var_2 = V * ( 1 - S * ( var_h - var_i ) );
    var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );

    if      ( var_i == 0 ) { 
      var_r = V     ; 
      var_g = var_3 ; 
      var_b = var_1 ;
    }
    else if ( var_i == 1 ) { 
      var_r = var_2 ; 
      var_g = V     ; 
      var_b = var_1 ;
    }
    else if ( var_i == 2 ) { 
      var_r = var_1 ; 
      var_g = V     ; 
      var_b = var_3 ;
    }
    else if ( var_i == 3 ) { 
      var_r = var_1 ; 
      var_g = var_2 ; 
      var_b = V     ;
    }
    else if ( var_i == 4 ) { 
      var_r = var_3 ; 
      var_g = var_1 ; 
      var_b = V     ;
    }
    else                   { 
      var_r = V     ; 
      var_g = var_1 ; 
      var_b = var_2 ;
    }

    R = (1-var_r) * 255;                  //RGB results = 0 ÷ 255
    G = (1-var_g) * 255;
    B = (1-var_b) * 255;
  }
}

int averagedRead() {
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(POT_PIN); 
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

