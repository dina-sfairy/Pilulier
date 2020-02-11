#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_MotorShield.h>


//Déclaration des variables et constantes
VL53L0X sensor1, sensor2; 
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *DClent;
Adafruit_DCMotor *DCrapide;
bool capteurPil, capteurPur, ready, enMarche, momentDone, PrescDone, samePil1, samePil2;
int tailleVect[4]; 
int pilParMoment[4];
int compteur1, compteur2, compteurTot, distancePil1, distancePil2, capPilPin, capPurPin;
int momentEnCours;
byte deplacement[21][4];
byte commande;
byte matricePrescription[7][4];
long timeActuel, timePilule;
//modifier les constantes suivantes pour les bonnes valeurs
const int DIST_SEUIL1 = 70;
const int DIST_SEUIL2 = 40;
const long TEMPS_MAX = 200;      //à changer après test
const int ADRESSE_COMP = 10;
const int ADRESSE_DIST = 11;
const int SENSOR1_XSHUNT_PIN = 12;
const int SENSOR2_XSHUNT_PIN = 13;
const uint8_t SENSOR1_ADDRESS = 5;
const uint8_t SENSOR2_ADDRESS = 6;

//Initialisation des variables
void initVar(){
    commande = 0;
    compteur1 = 0;
    compteur2 = 0;
    compteurTot = 0;
    timeActuel = 0;
    timePilule = 0;
    momentEnCours = 0;
    capteurPil = true; //à modifier quand on aura les vrais capteurs
    capteurPur = true; //à modifier quand on aura les vrais capteurs
    ready = true; //Mettre true pour les tests sans capteur de purge/pilulier
    enMarche = false;
    momentDone = false;
    PrescDone = false;
    samePil1 = false;
    samePil2 = false;
    memset(tailleVect, 0, sizeof(tailleVect));
    memset(pilParMoment, 0, sizeof(pilParMoment));
    memset(deplacement, 0, sizeof(deplacement));
}

void setup() {
    capPilPin = 1;  //à modifier
    capPurPin = 2;  //à modfier
    
    //Association des adresses des capteurs
    pinMode(SENSOR1_XSHUNT_PIN, OUTPUT);
    pinMode(SENSOR2_XSHUNT_PIN, OUTPUT);
    digitalWrite(SENSOR1_XSHUNT_PIN, LOW);
    digitalWrite(SENSOR2_XSHUNT_PIN, LOW);
    Serial.begin(115200);
    Wire.begin();
    //Set les adresses des capteurs
    setSensorsAddress();
    //mesure valeur référence des capteurs
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

    //Envoi de la commande de calibration - '20' correpond à la commande de calibration
    Wire.beginTransmission(ADRESSE_COMP);
    Wire.write(20);
    Wire.endTransmission();
    Wire.beginTransmission(ADRESSE_DIST);
    Wire.write(20);
    Wire.endTransmission();
    //UNCOMMENT Pour utiliser code de calibration
    /*
    while(Wire.requestFrom(ADRESSE_COMP,1)==0 && Wire.requestFrom(ADRESSE_DIST,1)==0){
        //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
    }
    */
    Serial.println("Les slaves sont calibres et ready");   //print pour les tests

    if (ready){
        //Tant qu'il n'y a pas de byte à lire au port sériel, le programme reste dans la boucle
        while(Serial.available()==0 && !enMarche){
        }
        commande = Serial.read();
        enMarche = true;
        //la commande 1 correspond à l'appui du bouton démarrer
        if (commande == 1){
            Serial.println("Commande de départ recue"); //print pour les tests
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
            Serial.println("Prescription recue et bien lue");  //print pour les tests
            Serial.print(tailleVect[0]);Serial.print(tailleVect[1]);Serial.print(tailleVect[2]);Serial.println(tailleVect[3]);  //print pour les tests                     //print pour les tests
            verifCommande();
            //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
            //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
            timePilule = millis();
            DClent->run(FORWARD); // Démarrer moteur lent de séparation

            //Code de remplissage de prescription
            //Boucle pour une prescription 
            while(PrescDone == false){
                verifCommande();
                //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                while(tailleVect[momentEnCours] == 0){
                    Serial.println("Moment ne contient pas de pilules"); 
                    momentEnCours++;
                }
                if(momentEnCours > 3){
                    Serial.println("prescription terminee"); //print pour les tests
                    PrescDone = true;
                    purgeComplete();
                    enMarche = false;
                    Serial.println("f");
                    break;
                }
                DCrapide->run(FORWARD);
                Serial.print("Debut moment de journée :"); Serial.println(momentEnCours); //print pour les tests
                timePilule = millis();
                while(momentDone == false){
                    verifCommande();
                    //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                    //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                    timeActuel = millis();
                    if((timeActuel-timePilule) > (TEMPS_MAX*1000)){
                        Serial.println(timeActuel);
                        Serial.println(timePilule);
                        Serial.println("e3");
                        while(Serial.available()==0){
                            //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                            //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                        }
                        verifCommande();
                    }
                    distancePil1 = sensor1.readRangeSingleMillimeters();
                    distancePil2 = sensor2.readRangeSingleMillimeters();
                    //Vérifie si une pilule passe devant le capteur1
                    if(distancePil1 < DIST_SEUIL1){
                        if(samePil1 == false){
                            samePil1 = true;
                            Serial.println("pilule captee par capteur 1"); //print pour les tests
                            verifCommande();
                            //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                            //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                            compteur1++;
                            timePilule = millis();
                            if(compteur1 == pilParMoment[momentEnCours]){
                                Serial.println("compteur 1 a atteint nb de pillules pour le moment"); //print pour les tests
                                DClent->run(RELEASE);
                            }
                        } else {
                            Serial.println("Meme pilule captee par capteur 1"); //Print pour les tests
                        }
                    }
                    if(distancePil1 > DIST_SEUIL1 + 10){
                        samePil1 = false;
                    }
                    
                    //Vérifie si une pilule passe devant le capteur2
                    if(distancePil2 < DIST_SEUIL2){
                        if(samePil2 == false){
                            samePil2 = true;
                            Serial.println("pilule captee par capteur 2"); //print pour les tests
                            verifCommande();
                            //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                            //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                            Wire.beginTransmission(ADRESSE_COMP);
                            Wire.write(deplacement[compteur2][momentEnCours]);
                            Wire.endTransmission();                        
                            compteur2++;
                            if(deplacement[momentEnCours][compteur2]==8){
                                Serial.println("derniere pilule a ete captee et envoie de commande de fin"); //print pour les tests
                                DCrapide->run(RELEASE);
                                momentDone = true; 
                                //Attend que cassette soit en place et qu'elle envoie son status
                    //            while(Wire.requestFrom(ADRESSE_DIST,1)==0){
                    //                verifCommande();
                    //                //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                    //                //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier 
                    //            }
                                Serial.println("cassette prete et en place"); //print pour les tests
                                Wire.beginTransmission(ADRESSE_COMP);
                                Wire.write(deplacement[compteur2][momentEnCours]);
                                Wire.endTransmission();
                                Serial.println("commande envoyer au slave compartimentation"); //print pour les tests
                            }
                        } else {
                            Serial.println("Meme pilule captee par capteur 2"); //Print pour les tests
                        }
                    }
                    if(distancePil2 > DIST_SEUIL2 + 10){
                        samePil2 = false;
                    }
                }
                //Attend que le tapis de compartimentation soit en place
        //        while(Wire.requestFrom(ADRESSE_COMP,1)==0){
        //            verifCommande();
        //            //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //            //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //        }
                Serial.println("tapis compartimentation est pret et en place"); //print pour les tests
                Wire.beginTransmission(ADRESSE_DIST);
                Wire.write(momentEnCours);
                Wire.endTransmission();
                Serial.println("commande envoyer au slave distribution"); //print pour les tests
                
                if(compteur1 > compteur2){
                    Serial.println("compteur 1 plus élevé que compteur 2"); //print pour les tests
                    compteurTot += compteur2;
                    compteur1 = compteur1 - compteur2;
                    compteur2 = 0;
                } else if(compteur2 > compteur1){
                    Serial.println("compteur 2 plus élevé que compteur 1"); //print pour les tests
                    compteurTot += compteur2;
                    purgePartielle(); // ???
                    compteur2 = 0;
                    compteur1 = 0;
                } else {
                    Serial.println("compteurs egaux"); //print pour les tests
                    compteurTot += compteur2;
                    compteur1 = 0;
                    compteur2 = 0;
                }
                momentDone = false;
                momentEnCours++;
                
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
}

void verifCommande(){
    if(Serial.available()!=0){
        commande = Serial.read();
        switch (commande)
        {
        case 2:
            arret();
            break;
        case 3: 
            timePilule = millis();
            Serial.println("Redemarrage reussi");       //print pour les tests
            break;
        default:
            Serial.println("Commande recue inconnue");  //print pour les tests
            arret();
            break;
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

void setSensorsAddress() {
  // Changer l'addresse du sensor1
  pinMode(SENSOR1_XSHUNT_PIN, INPUT);
  delay(100);
  sensor1.init(true);
  delay(100);
  sensor1.setAddress(SENSOR1_ADDRESS);
  delay(100);
  // Changer l'addresse du sensor2
  pinMode(SENSOR2_XSHUNT_PIN, INPUT);
  delay(100);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress(SENSOR2_ADDRESS);
  delay(100);
}
