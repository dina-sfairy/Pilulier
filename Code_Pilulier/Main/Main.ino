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
int compteur1 = 0, compteur2 = 0, compteurTot, distancePil1, distancePil2;
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
const int ADRESSE_COMP = 10;
const int ADRESSE_DIST = 11;

// Connecter un DC motor au port M1 et l'autre au port M3
Adafruit_DCMotor* DClent = AFMS.getMotor(1);
Adafruit_DCMotor* DCrapide = AFMS.getMotor(3);

void setup() {
    //mesure valeur référence des capteurs
    enMarche = false;

    // DC motor tapis lent
    DClent->setSpeed(200); // À déterminer

    // DC motor tapis rapide
    DCrapide->setSpeed(300); // À déterminer
}

void loop(){
    //Tant que le pilulier et la purge ne sont pas bien en place le système ne commence pas
    while(!ready){
        if (digitalRead(capPilPin)){capteurPil = true;}
        else {
          Serial.println("e1");
          Serial.println("Veuillez bien insérer le pilulier");
        }
        if (digitalRead(capPurPin)){capteurPur = true;}
        else {
          Serial.println("e2");
          Serial.println("Veuillez bien insérer le récipient de purge");
          }
        if (capteurPil && capteurPur){ready = true;}
    }

    //CODE DE CALIBRATION : si on veut ajouter une calibration -> TODO

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
                //Calcul du nombre de pilules par moment
                pilParMoment[i] = tailleVect[i] - 1;
            }
            //Acquisition des vecteurs déplacements de chaque moment
            for(int j = 0; j<4;j++){
                for(int k = 0; k<tailleVect[j];k++){
                    deplacement[k][j] = Serial.read();
                }
            }
            
            timePilule = milis();
            DClent->run(FORWARD); // Démarrer moteur lent de séparation

            //Code de remplissage de prescription
            //Boucle pour une prescription 
            while(PrescDone == false){
                while(tailleVect[momentEnCours] == 0){
                    momentEnCours++;
                }
                DCrapide->run(FORWARD);
                while(momentDone == false){
                    timeActuel = milis();
                    if(timeActuel-timePilule >= TEMPS_MAX*1000){
                        Serial.println("e3");
                        //BEIKS Boucle de lecture du port sériel en attente du signal de reprise - surtout reset timer
                        //si on fait ça -> TODO
                        //timePilule = milis();
                    }
                    distancePil1 = sensor1.readRangeSingleMillimeters();
                    distancePil2 = sensor2.readRangeSingleMillimeters();
                    //Vérifie si une pilule passe devant le capteur1
                    if(DIST_REF1 - distancePil1 > DIST_SEUIL1){
                        compteur1++;
                        timePilule = milis();
                        if(compteur1 == pilParMoment[momentEnCours]){
                            DClent->run(RELEASE);
                            // Il faut que le bouton 'Redémarrer' recommence le moteur -> TODO
                        }
                    }
                    //Vérifie si une pilule passe devant le capteur2
                    if(DIST_REF2 - distancePil2 > DIST_SEUIL2){
                        //BEIKS send.deplacement[compteur2][momentEnCours] au slave compartimentation -> TODO
                        compteur2++;
                        if(deplacement[momentEnCours][compteur2]==8){
                            DCrapide->run(RELEASE);
                            momentDone = true;
                            //BEIKS boucle while byte sur état de cassette est pas positif lecture et attente -> TODO
                            //une fois reçu --> Wire.requestFrom(ADRESSE, 1);
                            
                            //send.deplacement[momentEnCours][compteur2] (i.e. 8) au slave compartimentation -> TODO
                            //Wire.beginTransmission(ADRESSE_COMP)
                            //Wire.write(int)
                            //Wire.endTransmission()
                        } 
                    }
                }
                //BEIKS send momentEnCours au slave cassette -> TODO
                //Wire.beginTransmission(ADRESSE_DIST)
                //Wire.write(int)
                //Wire.endTransmission()
                if(compteur1 > compteur2){
                    compteurTot += compteur2;
                    compteur1 = compteur1 - compteur2;
                    compteur2 = 0;
                } else if(compteur2 > compteur1){
                    compteurTot += compteur2;
                    purgePartielle() // ???
                    //sinon stop moteur lent
                    //compteur2 = 0;
                    //compteur1 = 0;
                } else {
                    compteurTot += compteur2;
                    compteur1 = 0;
                    compteur2 = 0;
                }
                momentDone = false;
                momentEnCours++;
                if(momentEnCours > 3){
                    PrescDone = true;
                    purgeComplete();
                    Serial.println("f");
                    break;
                }
            }
        }
    }
}

void purgePartielle() {
  DClent->run(RELEASE);
  DCrapide->run(BACKWARD); // pour combien de temps?
  }

void purgeComplete() {
  DClent->setSpeed(500); // À déterminer
  DCrapide->run(BACKWARD); // pour combien de temps?
  }
