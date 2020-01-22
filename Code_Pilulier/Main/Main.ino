/*
 Name:		Main.ino
 Created:	21/01/2020
 Author:	Beiko
*/




#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <VL53L0X.h> 

bool capteurPil, capteurPur, ready, enMarche;
int capPilPin = 1; //à modifier
int capPurPin = 2; //à modifier
int tailleVect[4]; 
byte deplacement[21][4];
byte commande;
byte matricePrescription[7][4];
long timeDepart, timePilule;

void lirePrescription() {
  for (byte i = 0; i < 7; i++) {
    for (byte j = 0; j < 4; j++) {
      matricePrescription[i][j] = Serial.read();
    }
  }
}

void setup() {
    //déterminer adresse capteur
    //mesure valeur référence des capteurs
    enMarche = false;
}

void loop(){
    while(!ready){
        if (digitalRead(capPilPin)){capteurPil = true;}
        if (digitalRead(capPurPin)){capteurPur = true;}
        if (capteurPil && capteurPur){ready = true;}
    }

    if (ready){
        while(Serial.available()==0 && !enMarche){
        }
        commande = Serial.read()
        enMarche = true;
        if (commande == 1){
            delay(10);
            for(int i = 0; i<4; i++){
                tailleVect[i] = Serial.read();
            }
            for(int j = 0; j<4;j++){
                for(int k = 0; k<tailleVect[j];k++){
                    deplacement[k][j] = Serial.read();
                }
            }
            lirePrescription();
            timeDepart = milis()
            //départ moteur
            //code de départ
            for(int i = 0; i <4; i++){

            }
            

        }

    }
}