void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

bool checkForCancel() {
  if (Serial.available()) {
    int y = Serial.read();
    if (y == 2) {
      Serial.println("ok");
      return true;
    }

  }
  return false;
}

void lireMatrice(bool lire_1) {
  if (lire_1) {
    Serial.read();
  }
  for (int i = 0; i < 28; i++) {
    Serial.read();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() == 0) {} //Fait rien tant que tu reçois rien
  randomSeed(millis());

  delay(10);

  lireMatrice(true);

  bool fin = false;
  while (fin != true) {
    delay(3000);
    fin = checkForCancel();
    if (fin) {
      break;
    }
    int nbrAleatoire = random(0, 2);
    if (nbrAleatoire == 0) {
      Serial.println("e3");
      delay(1000);
      byte x = 10;
      while (x != 3) {
        x = Serial.read();
        //        if (x == 2) {
        //          fin = true;
        //          Serial.println("ok");
        //          break;
        //        }
      }
    } else {
      Serial.println("f");
      byte z = Serial.read();
      switch (z) {
        case 1:
          lireMatrice(false);
          break;
        case 2:
          fin = true;
          Serial.println("ok");
          break;
        case 4:
          fin = true;
          break;
      }
    }
  }

  while (true) {}

}
