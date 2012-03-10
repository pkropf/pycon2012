void setup() {
  Serial.begin(57600);
}

void loop() {
  int incomingByte = 0;

  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
}

