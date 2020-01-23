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

VL53L0X sensor1, sensor2; 
bool capteurPil, capteurPur, ready, enMarche, momentDone, PrescDone;
int capPilPin = 1; //à modifier
int capPurPin = 2; //à modifier
int tailleVect[4]; 
int pilParMoment[4];
int compteur1 = 0, compteur2 = 0, distancePil1, distancePil2;
int momentEnCours = 0;
byte deplacement[21][4];
byte commande;
byte matricePrescription[7][4];
long timeActuel, timePilule;
//modifier les constantes suivantes pour les bonnes valeurs
const int DIST_REF1 = 20;
const int DIST_REF2 = 20;
const int DIST_SEUIL1 = 10;
const int DIST_SEUIL2 = 10;
const int TEMPS_MAX = 10;

//pas nécessaire finalement je crois
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
    //Tant que le pilulier et la purge ne sont pas bien en place le système ne commence pas
    while(!ready){
        if (digitalRead(capPilPin)){capteurPil = true;}
        if (digitalRead(capPurPin)){capteurPur = true;}
        if (capteurPil && capteurPur){ready = true;}
        //envoie message à l'interface pour dire que les trucs sont pas en place?
    }

    if (ready){
        //Tant qu'il n'y a pas de byte à lire au port sériel, le programme reste dans la boucle
        while(Serial.available()==0 && !enMarche){
        }
        commande = Serial.read();
        enMarche = true;
        //la commande 1 correspond à l'appui du bouton démarrer
        if (commande == 1){
            delay(10);
            //Acquisition du vecteur de taille des vecteurs déplacement/moment
            for(int i = 0; i<4; i++){
                tailleVect[i] = Serial.read();
            }
            //Acquisition des vecteurs déplacements de chaque moment
            for(int j = 0; j<4;j++){
                for(int k = 0; k<tailleVect[j];k++){
                    deplacement[k][j] = Serial.read();
                }
            }
            //Calcul du nombre de pilules par moment
            for (int i = 0; i < 4; i++){
                for (int k = 0; k < tailleVect[i]; k++){
                    pilParMoment[i] += deplacement[i][k]; 
                }
                pilParMoment[i] -= 8; //si on garde le 8 à la fin des mvts pour signaler la fin
            }
            
            //lirePrescription();
            timePilule = milis();
            //départ moteur TODO : soit fct ou bien on fait juste envoyer la commande au moteur
            //Code de remplissage de prescription
            //Boucle pour une prescription 
            while(PrescDone == false){
                while(momentDone == false){
                    timeActuel = milis();
                    if(timeActuel-timePilule >= TEMPS_MAX*1000){
                        //Alerte, message interface manque de pilule -> TODO
                    }
                    distancePil1 = sensor1.readRangeSingleMillimeters();
                    distancePil2 = sensor2.readRangeSingleMillimeters();
                    if(DIST_REF1 - distancePil1 > DIST_SEUIL1){
                        compteur1++;
                        timePilule = milis();
                        if(compteur1 == pilParMoment[momentEnCours]){
                            //fermer le moteur lent -> TODO
                        }
                    }
                    if(DIST_REF2 - distancePil2 > DIST_SEUIL2){
                        //send.deplacement[momentEnCours][compteur2] au slave compartimentation -> TODO
                        if(deplacement[momentEnCours][compteur2]==8){
                            momentDone = true;
                        }
                        compteur2++;
                    }
                }
                //send momentEnCours au slave cassette -> TODO
                momentEnCours++;
                if(momentEnCours > 3){
                    PrescDone = true;
                }
            }
            
            

        }

    }
}