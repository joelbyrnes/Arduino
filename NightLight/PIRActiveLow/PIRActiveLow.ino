int motion_1 = 3;
int light_1 = 13;
void setup(){
  pinMode (motion_1,INPUT);
  pinMode (light_1, OUTPUT);
}

void loop (){
  digitalWrite (light_1,LOW);
  delay(1000); //this delay is to let the sensor settle down before taking a reading
  int sensor_1 = digitalRead(motion_1);
  if (sensor_1 == LOW){
    digitalWrite(light_1,HIGH);
    delay(500);
    digitalWrite(light_1,LOW);
    delay(500);

  }
}
