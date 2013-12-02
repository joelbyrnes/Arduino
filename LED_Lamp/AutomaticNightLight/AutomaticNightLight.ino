// Automated Runner Lights (v1.0)
// Scott Martin & Todd Medema
// http://fabricate.io
// 10/3/2013
//
// Change this code to your heart's content!

// http://www.instructables.com/id/Automatic-Hallway-Runner-Lights/step2/Program-the-Arduino/

// Constants won't change. They're used here to set pin numbers and thresholds:
const int motionPin = A3;
const int lightAnalogPin = A0;
const int ledPin = 10; 
const int LIGHT_THRESH = 800;
const int MOTION_THRESH = 100;
const int TIMEOUT = 1000;
const boolean invertPWM = true;

// variables will change:
int motionState = 0; // countdown after last motion
int lightState = 0; // countdown after last dark
int LEDActive = 0; // if LEDs are on
int LEDBrightness = 0; // current LED brightness (for fade in/out)

void setup() {
  // lights off to start with
  setLight(0);
  pinMode(ledPin, OUTPUT);

  // power LDR resistor on 12
  pinMode(12, OUTPUT);
  digitalWrite(12, 1);
  
  // set up input pins
  pinMode(lightAnalogPin, INPUT);
  pinMode(motionPin, INPUT);
    
  Serial.begin(57600);
  
//  Serial.print("waiting for PIR to warm up... ");
//  delay(3000);
//  Serial.println("ready");
  
}

void loop(){
  if (analogRead(lightAnalogPin) < LIGHT_THRESH) { //Is Bright
    lightState = 0;
  } else {
    lightState = TIMEOUT;
  }
  
  if (lightState && analogRead(motionPin) < MOTION_THRESH) {
    motionState = TIMEOUT;
  }
 
  if (motionState > 0) motionState -= 1;
  if (lightState > 0) lightState -= 1;
 
  Serial.print("cell: ");
  Serial.print(analogRead(lightAnalogPin));
  Serial.print(",\t pir: ");
  Serial.print(analogRead(motionPin));

  if (motionState && (lightState || LEDActive)) {    
    LEDActive = true;
  }
  else {
    LEDActive = false;
  }
  if (LEDActive) {
    if (LEDBrightness < 255) {
      LEDBrightness = min(LEDBrightness + 1, 255);
    }
  } else {
    if (LEDBrightness > 0) {
      LEDBrightness = max(LEDBrightness - 1, 0);
    }
  }
  
  // use parabola to bring up brightness more slowly at first
  // y = x^2 / 255
  int parabolicBrightness = int((double(LEDBrightness) * double(LEDBrightness)) / 255.0);

  Serial.print(",\t motion: ");
  Serial.print(motionState);
  
  Serial.print(",\t light: ");
  Serial.print(lightState);
  
  Serial.print(",\t led: ");
  Serial.println(parabolicBrightness);

  setLight(parabolicBrightness);
  
  delay(20);
}

void setLight(int value) {
  if (value > 255) value = 255;
  if (value < 0) value = 0;
  if (invertPWM) analogWrite(ledPin, 255 - value);
  else analogWrite(ledPin, value);
}

