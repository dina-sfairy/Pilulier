
byte matricePrescription[7][4];
void lirePrescription() {
  for (byte i = 0; i < 7; i++) {
    for (byte j = 0; j < 4; j++) {
      matricePrescription[i][j] = Serial.read();
    }
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    lirePrescription();
  }

}
