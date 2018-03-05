

void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial3.begin(19200);
}

void loop() {
  // read from port A, send to port B:
  if (Serial3.available()) {
    int inByte = Serial3.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial3.write(inByte);
  }
}
