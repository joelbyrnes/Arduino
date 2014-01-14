#include <ShiftRegLCD123.h>
#include "DHT.h"

#define LCD_DATA_PIN  3     // PB3, attiny85 ADC3, physical pin 2
#define LCD_CLOCK_PIN 4     // PB4, attiny85 ADC2, physical pin 3
#define DHT11_PIN     0     // PB0, attiny85 PCINT0, physical pin 5

// Instantiate a LCD object
ShiftRegLCD123 srlcd(LCD_DATA_PIN, LCD_CLOCK_PIN, SRLCD123);

DHT dht;

void setup() {  
  // Initialize the LCD and set display size
  // LCD size 20 columns x 2 lines, defaults to small (normal) font
  srlcd.begin(16,1);
  
  // Turn on backlight (if used)
  srlcd.backlightOn();
  
  dht.setup(DHT11_PIN); 
}

double humid, temp;

void loop() {
  delay(dht.getMinimumSamplingPeriod());
  humid = dht.getHumidity();
  temp = dht.getTemperature();

  srlcd.clear();
  
  // check for "OK" / ERROR_NONE
  if (dht.getStatus()) {
    srlcd.print(dht.getStatusString());
    delay(1000);
    srlcd.clear();
    
  } else {

    srlcd.print("T ");
    srlcd.print(temp);
    srlcd.print("C");
    delay(1000);
  
    srlcd.clear();
    srlcd.print("H ");
    srlcd.print(humid);
    srlcd.print("%");
    delay(1000);
    
    srlcd.clear();
    srlcd.print("D ");
    srlcd.print(dewPoint(temp, humid));
    srlcd.print("C");
  }
}

// below from http://arduino-info.wikispaces.com/DHT11-Humidity-TempSensor

//Celsius to Kelvin conversion
double Kelvin(double celsius) {
        return celsius + 273.15;
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity) {
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity) {
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}

