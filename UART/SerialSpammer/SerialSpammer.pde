  
void setup() {
  Serial.begin(2400);
}

void loop() {
  sendIRBeacon();
}

void sendIRBeacon() {
  Serial.print("ABC\n");
  delay(100);
}

void sendLongString() {
  // this is 26 * 3 + 1 = 69 bytes so it wraps the 64 byte buffer and should be easy to see bits missing. 

  for (int i = 0; i < 10; i++) {
    Serial.print('-');
    Serial.print(i);
    Serial.print("-Aa.Bb.Cc.Dd.Ee.Ff.Gg.Hh.Ii.Jj.Kk.Ll.Mm.Nn.Oo.Pp.Qq.Rr.Ss.Tt.Uu.Vv.Ww.Xx.Yy.Zz.\n");
  }
    
  delay(2000);
}
