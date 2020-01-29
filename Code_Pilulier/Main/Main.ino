#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_MotorShield.h>

VL53L0X sensor1, sensor2; 
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *DClent;
Adafruit_DCMotor *DCrapide;
bool capteurPil, capteurPur, ready, enMarche, momentDone, PrescDone;
int capPilPin = 1; //à modifier
int capPurPin = 2; //à modifier
int tailleVect[4]; 
int pilParMoment[4];
int compteur1, compteur2, compteurTot, distancePil1, distancePil2;
int momentEnCours;
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

//Initialisation des variables
void initVar(){
    compteur1 = 0;
    compteur2 = 0;
    compteurTot = 0;
    timeActuel = 0;
    timePilule = 0;
    momentEnCours = 0;
    capteurPil = false;
    capteurPur = false;
    ready = false;
    enMarche = false;
    momentDone = false;
    PrescDone = false;
    memset(tailleVect, 0, sizeof(tailleVect));
    memset(pilParMoment, 0, sizeof(pilParMoment));
    memset(deplacement, 0, sizeof(deplacement));
}

void setup() {
    //mesure valeur référence des capteurs
    //associer les sensors aux capteurs
    enMarche = false;
    initVar();

    //Connecter un DC motor au port M1 et l'autre au port M3
    DClent = AFMS.getMotor(1);
    DCrapide = AFMS.getMotor(3);
    // DC motor tapis lent
    DClent->setSpeed(200); // À déterminer
    // DC motor tapis rapide
    DCrapide->setSpeed(300); // À déterminer
}

void loop(){
    //Réinitialise les valeurs des variables
    initVar();
    //Tant que le pilulier et la purge ne sont pas bien en place le système ne commence pas
    while(ready == false){
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
            verifArret();
            verifPil();
            verifPurge();
            timePilule = millis();
            DClent->run(FORWARD); // Démarrer moteur lent de séparation

            //Code de remplissage de prescription
            //Boucle pour une prescription 
            while(PrescDone == false){
                verifArret();
                verifPil();
                verifPurge();
                while(tailleVect[momentEnCours] == 0){
                    momentEnCours++;
                }
                DCrapide->run(FORWARD);
                while(momentDone == false){
                    verifArret();
                    verifPil();
                    verifPurge();
                    timeActuel = millis();
                    if(timeActuel-timePilule >= TEMPS_MAX*1000){
                        Serial.println("e3");
                        while(Serial.available()==0){
                            verifArret();
                            verifPil();
                            verifPurge();
                        }
                        commande = Serial.read();
                        if(commande == 3){
                            timePilule = millis();
                        } //else envoyer message à l'interface pour commande erronnée?
                    }
                    distancePil1 = sensor1.readRangeSingleMillimeters();
                    distancePil2 = sensor2.readRangeSingleMillimeters();
                    //Vérifie si une pilule passe devant le capteur1
                    if(DIST_REF1 - distancePil1 > DIST_SEUIL1){
                        verifArret();
                        verifPil();
                        verifPurge();
                        compteur1++;
                        timePilule = millis();
                        if(compteur1 == pilParMoment[momentEnCours]){
                            DClent->run(RELEASE);
                        }
                    }
                    //Vérifie si une pilule passe devant le capteur2
                    if(DIST_REF2 - distancePil2 > DIST_SEUIL2){
                        verifArret();
                        verifPil();
                        verifPurge();
                        Wire.beginTransmission(ADRESSE_COMP);
                        Wire.write(deplacement[compteur2][momentEnCours]);
                        Wire.endTransmission();                        
                        compteur2++;
                        if(deplacement[momentEnCours][compteur2]==8){
                            DCrapide->run(RELEASE);
                            momentDone = true; 
                            //Attend que cassette soit en place et qu'elle envoie son status
                            while(Wire.requestFrom(ADRESSE_DIST,1)==0){
                                verifArret();
                                verifPil();
                                verifPurge();  
                            }
                            Wire.beginTransmission(ADRESSE_COMP);
                            Wire.write(deplacement[compteur2][momentEnCours]);
                            Wire.endTransmission();
                        } 
                    }
                }
                //Attend que le tapis de compartimentation soit en place
                while(Wire.requestFrom(ADRESSE_COMP,1)==0){
                    verifArret();
                    verifPil();
                    verifPurge();
                }
                Wire.beginTransmission(ADRESSE_DIST);
                Wire.write(momentEnCours);
                Wire.endTransmission();
                
                if(compteur1 > compteur2){
                    compteurTot += compteur2;
                    compteur1 = compteur1 - compteur2;
                    compteur2 = 0;
                } else if(compteur2 > compteur1){
                    compteurTot += compteur2;
                    purgePartielle(); // ???
                    compteur2 = 0;
                    compteur1 = 0;
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
  DCrapide->run(BACKWARD); 
  delay(5000); // Temps à déterminer
  DCrapide->run(RELEASE);
}

void purgeComplete() {
  DClent->setSpeed(500); // À déterminer
  DClent->run(FORWARD); // À voir si on a besoin de run le moteur après avoir changer la vitesse
  DCrapide->run(BACKWARD); 
  arret();
  //fermer les deux moteurs à la fin - à voir selon quand on demande une purge complète
}

void arret(){
    DClent->run(RELEASE);
    DClent->run(RELEASE);
    Serial.println("ok");
    Serial.println("Le systeme est arrete!");
}

void verifArret(){
    if(Serial.available()!=0){
        commande = Serial.read();
        if(commande == 2){
        arret();
        }
    }
}

void verifPurge(){
    if(digitalRead(capPurPin)){
        arret();
        Serial.println("e2");
        Serial.println("Veuillez bien insérer le récipient de purge");
        ready = false;
    }
}

void verifPil(){
    if(digitalRead(capPilPin)){
        arret();
        Serial.println("e1");
        Serial.println("Veuillez bien insérer le pilulier");
        ready = false;
    }
}
