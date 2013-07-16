/*
 Rainbow

 Take a tricolour LED through the colours of the rainbow using an analogue input.

 The circuit:
 * TBC

 Created 21 April 2012
 By Jason Judge

 This example code is in the public domain.

 */

// The three primary colour LEDs, driven as analgue outputs (actually PWM, but
// close enough for our analogue eyes).

const int ledPinRed = 9;    // Red LED connected to analogue out pin
const int ledPinGrn = 10;    // Green LED connected to analogue out pin
const int ledPinBlu = 11;     // Blue LED connected to analogue out pin

const int modeSwitchPin = 12;

// The Hue potentiometer goes on an analogue pin, taking the pin from
// 0V to 5V.

const int potPinHue = 0;

// Constants to define the ranges.

const int hueRedLow = 0;
const int hueRedHigh = 255;
const int hueBlue = 170;

// The size of the angle of one sector (1/6 of a colour wheel), and of a complete
// cycle of the colour wheel.

const int angleMin = 0;
const int angleSector = 60;
const int angleMax = 360;

const int brightMin = 0;
const int brightMax = 255;

// Work variables.

// The potentiometer value is mapped to the range 0 to 360 (degrees).
int potValueHue;

// The hue is the range 0 (red) to 170 (blue) in rainbow
// mode or 255 (red) in colour wheel mode.
// The brightness ranges from 0 (dark) to 255 (full brightness)

int hue, brightness;

// The saturation is fixed at 255 (full) to remove blead-through of different
// colours.
// It could be linked to another potentiometer if a demonstration of hue
// is desired.

const int saturation = 255;

// The brightess of each LED (0 to 255).

unsigned int r, g, b;

void setup()  {
    // Still need to set a baud rate, even for USB.
    Serial.begin(9600);

    // Set LED pins to output.
    pinMode(ledPinRed, OUTPUT);
    pinMode(ledPinGrn, OUTPUT);
    pinMode(ledPinBlu, OUTPUT);

    // Poteniometer analogue pin is an input.
    pinMode(potPinHue, INPUT);

    // TODO: mode switch in a digital input.
    pinMode(modeSwitchPin, INPUT);
}

void loop()  {
    // The Hue potentiometer value is mapped to degrees - 0 to 360 - for convenience.
    potValueHue = map(analogRead(potPinHue), 0, 1023, 0, 360);

    // Control the mode using a physical switch.

    if (true) {
    //if (digitalRead(modeSwitchPin)) {
        // Rainbow colour mode (infra-red to ultra-violet - well invisible to invisible  .

        // The hue ranges from red (0) at 60 degrees to blue (170) at 300 degrees.
        hue = constrain(map(potValueHue, angleSector, angleMax - angleSector, hueRedLow, hueBlue), hueRedLow, hueBlue);

        // Brightness - fade up 0-60 degrees
        brightness = constrain(map(potValueHue, angleMin, angleSector, brightMin, brightMax), brightMin, brightMax);

        // Brightness fade down 300-360 degrees
        brightness = brightness - constrain(map(potValueHue, angleMax - angleSector, angleMax, brightMin, brightMax), brightMin, brightMax);
        
        delay(10);
    } else {
        // Colour wheel mode (red to red, wrapped around in a cycle).

        hue = map(potValueHue, angleMin, angleMax, hueRedLow, hueRedHigh);

        // The brightness is fixed at full for the colour wheel. This could be
        // linked to another poteniometer if that is a concept you wish to
        // demonstrate.
        brightness = 255;
    }

    // Do the conversion.
    HSBToRGB(hue, saturation, brightness, &r, &g, &b);

    analogWrite(ledPinRed, r);
    analogWrite(ledPinGrn, g);
    analogWrite(ledPinBlu, b);

    Serial.print(" bright=");
    Serial.print(brightness);
    Serial.print(" hue=");
    Serial.print(hue);
    Serial.print(" red=");
    Serial.print(r);
    Serial.print(" green=");
    Serial.print(g);
    Serial.print(" blue=");
    Serial.print(b);
    Serial.println("");
    delay(50);
}

// This function taken from here:
// http://eduardofv.com/read_post/179-Arduino-RGB-LED-HSV-Color-Wheel-

void HSBToRGB(
    unsigned int inHue, unsigned int inSaturation, unsigned int inBrightness,
    unsigned int *oR, unsigned int *oG, unsigned int *oB )
{
    if (inSaturation == 0)
    {
        // achromatic (grey)
        *oR = *oG = *oB = inBrightness;
    }
    else
    {
        unsigned int scaledHue = (inHue * 6);
        unsigned int sector = scaledHue >> 8; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 8);	// position within the sector         
        unsigned int p = (inBrightness * ( 255 - inSaturation )) >> 8;
        unsigned int q = (inBrightness * ( 255 - ((inSaturation * offsetInSector) >> 8) )) >> 8;
        unsigned int t = (inBrightness * ( 255 - ((inSaturation * ( 255 - offsetInSector )) >> 8) )) >> 8;

        switch( sector ) {
        case 0:
            *oR = inBrightness;
            *oG = t;
            *oB = p;
            break;
        case 1:
            *oR = q;
            *oG = inBrightness;
            *oB = p;
            break;
        case 2:
            *oR = p;
            *oG = inBrightness;
            *oB = t;
            break;
        case 3:
            *oR = p;
            *oG = q;
            *oB = inBrightness;
            break;
        case 4:
            *oR = t;
            *oG = p;
            *oB = inBrightness;
            break;
        default:    // case 5:
            *oR = inBrightness;
            *oG = p;
            *oB = q;
            break;
        }
    }
}
