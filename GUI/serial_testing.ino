void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    byte x = Serial.read();
    if (x == 10) {
      for (byte i = 0; i < 10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        if (Serial.available()) {
          byte y = Serial.read();
          if (y == 11) {
              break;
            }
        }
      }
      Serial.println("Arret du systeme");
    }
  }

}
