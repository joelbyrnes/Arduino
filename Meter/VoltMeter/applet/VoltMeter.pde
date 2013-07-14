// from http://code.google.com/p/tinkerit/wiki/SecretVoltmeter


long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  Serial.print("REFS0: ");
  Serial.println( REFS0, DEC );
  Serial.print("ADMUX: ");
  Serial.println( ADMUX, DEC );
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  Serial.print("ADCSRA: ");
  Serial.println( ADCSRA, DEC );
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println( readVcc(), DEC );
  delay(1000);
}

