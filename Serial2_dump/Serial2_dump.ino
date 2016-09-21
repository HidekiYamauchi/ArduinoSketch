
//

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println("RESET");
  Serial.println();

}

void loop() {

  if(Serial2.available()){
    Serial.write(Serial2.read());
  }

}
