
int redPin = 9;
int grnPin = 10;
int bluPin = 11;

#define SIZE    255
#define DELAY    20
#define HUE_MAX  360.0
#define HUE_DELTA 0.1
#define L_MAX  1.0
#define L_DELTA 0.01

float bright[3] = { 255., 255., 255.};

void setup() {
  pinMode(redPin, OUTPUT);  
  pinMode(grnPin, OUTPUT);  
  pinMode(bluPin, OUTPUT);  
  //colour(64, 64, 64);
  setHSLColour(0., 1.0, 1.0);
  delay(500);
  
  Serial.begin(115200);
}

void loop() {
  //hueloop();
  //lightnessloop();

  readHSL();
}

void readHSL() {
  while (1) {
    double h = double(readByte())/256. * 360;
    double s = double(readByte())/256.;
    double l = double(readByte())/256.;
    Serial.print(h);
    Serial.print(" ");
    Serial.print(s);
    Serial.print(" ");
    Serial.println(l);
    setHSLColour(h, s, l);
  }  
}
  
// TODO this function will probably fill up the stack if it gets a lot of bad values. 
byte readByte() {
  while (Serial.available() <= 0) {
    delayMicroseconds(1000);
  }
  int value = Serial.read();
  if (value != -1) {
    return byte(value);
  } else {
    return readByte();
  }
}

long rgb[3];
long rgbval;
double hue=0.0, saturation=1.0, lightness=0.5;

long k, temp_value;

void lightnessloop() {
  lightness += L_DELTA;
  if (lightness > L_MAX) {
    lightness=0.0;
  }
//  Serial.println(lightness);
  setHSLColour(hue, saturation, lightness);

  delay(20);
}

void hueloop() {
  hue += HUE_DELTA;
  if (hue > HUE_MAX) {
    hue=0.0;
  }
//  Serial.println(hue);
  setHSLColour(hue, saturation, lightness);

  delay(1);
}

// https://gist.github.com/xpansive/1337890
void setHSLColour(double hue, double sat, double light) {
  sat *= light <.5? light : 1-light;
 
  double v = light+sat;
  //setHSVColour(hue, 2*sat/v, v);
  setLedColorHSV(hue, 2*sat/v, v);
}

void setHSVColour(double h, double s, double v) {
  double r,g,b;
  HSVtoRGB(&r, &g, &b, h, s, v);
  colour(r * bright[0], g * bright[1], b * bright[2]);
}

void colour(byte red, byte grn, byte blu) {
  analogWrite(redPin, red);
  analogWrite(grnPin, grn);
  analogWrite(bluPin, blu);
}

// from http://www.cs.rit.edu/~ncs/color/t_convert.html 

void HSVtoRGB( double *r, double *g, double *b, double h, double s, double v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

/*
  Serial.print(i);
  Serial.print(" ");  
  Serial.print(p);
  Serial.print(" ");  
  Serial.print(q);
  Serial.print(" ");
  Serial.print(t);
  Serial.print(" ");
  Serial.println(v);
*/
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
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
  byte red=constrain((int)bright[0]*r,0,255);
  byte green=constrain((int)bright[1]*g,0,255);
  int blue=constrain((int)bright[2]*b,0,255);

  colour(red,green,blue);
}


