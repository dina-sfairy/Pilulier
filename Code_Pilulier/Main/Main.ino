#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_VL6180X.h>

//Déclaration des variables et constantes
VL53L0X sensor1, sensor2; 
//Adafruit_VL6180X sensorBleu = Adafruit_VL6180X();
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *DClent;
Adafruit_DCMotor *DCrapide;
bool capteurPil, capteurPur, capteurCompart, momentDone, PrescDone, samePil1, samePil2, recuComp, recuDist;
int tailleVect[4]; 
int pilParMoment[4]; 
int compteur1, compteur2, compteurTot, distancePil1, distancePil2, capPilPin, capPurPin, momentEnCours;
byte deplacement[21][4];
byte commande;
byte matricePrescription[7][4];
long timeActuel, timePilule;
//modifier les constantes suivantes pour les bonnes valeurs
const int DIST_SEUIL1 = 60;
const int DIST_SEUIL2 = 35;
const int DIST_SEUIL3 = 25; // À modifier après tests
const long TEMPS_MAX = 200; //à changer après test
const int ADRESSE_COMP = 10;
const int ADRESSE_DIST = 11;
//const int SENSOR_BLEU_SHUT_PIN = 11;
const int SENSOR1_XSHUNT_PIN = 12;
const int SENSOR2_XSHUNT_PIN = 13;
const uint8_t SENSOR1_ADDRESS = 5;
const uint8_t SENSOR2_ADDRESS = 6;

//Initialisation des variables
void initVar(){
    compteur1 = 0;
    compteur2 = 0;
    compteurTot = 0;
    timeActuel = 0;
    timePilule = 0;
    momentEnCours = 0;
    capteurPil = true; //à modifier quand on aura les vrais capteurs
    capteurPur = true; //à modifier quand on aura les vrais capteurs
    //capteurCompart = false;
    momentDone = false;
    PrescDone = false;
    samePil1 = false;
    samePil2 = false;
    recuDist = false;
    recuComp = false;
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
    //pinMode(SENSOR_BLEU_SHUT_PIN, OUTPUT);
    digitalWrite(SENSOR1_XSHUNT_PIN, LOW);
    digitalWrite(SENSOR2_XSHUNT_PIN, LOW);
    //digitalWrite(SENSOR_BLEU_SHUT_PIN, LOW);
    Serial.begin(115200);
    Wire.begin();
    //Set les adresses des capteurs
    setSensorsAddress();
    initVar();
    //Connecter un DC motor au port M1 et l'autre au port M3
    AFMS.begin();
    DClent = AFMS.getMotor(1);
    DCrapide = AFMS.getMotor(3);
    DClent->setSpeed(200); // À déterminer
    DCrapide->setSpeed(150); // À déterminer
}

void loop(){
    DClent->run(RELEASE);
    DCrapide->run(RELEASE);
    //Réinitialise les valeurs des variables
    initVar();
    //Tant que le pilulier et la purge ne sont pas bien en place le système ne commence pas
    //verifPurge();
    //verifPil();

    //Tant que les salves ne sont pas prêt le système ne commence pas
    recuComp = false;
    recuDist = false;
    //while(!recuComp && !recuDist && capteurCompart){ 
    while(!recuComp && !recuDist) {
        Wire.requestFrom(ADRESSE_COMP,1);
        recuComp = Serial.read();
        Wire.requestFrom(ADRESSE_DIST,1);
        recuDist = Serial.read();
        //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //verifCompart(); Calibration
    }
    Serial.println("Les slaves sont calibres et ready");   //print pour les tests

    //Tant qu'il n'y a pas de byte à lire au port sériel, le programme reste dans la boucle
    while(Serial.available()==0){
    }
    commande = Serial.read();
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
        verifCommande();
        //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
        timePilule = millis();
        DClent->run(FORWARD); // Démarrer moteur lent de séparation
        DCrapide->run(FORWARD);
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
                Serial.println("type de pilule termine"); //print pour les tests
                PrescDone = true;
                purgeComplete();
                Serial.println("f");
                break;
            }
            Serial.print("Debut moment de journée :"); Serial.println(momentEnCours); //print pour les tests
            timePilule = millis();
            while(momentDone == false){
              //capteurCompart = false;
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
                        Serial.println("pilule captee par capteur 2, deplacement ="); //print pour les tests
                        Serial.println(deplacement[compteur2][momentEnCours]);
                        verifCommande();
                        //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                        //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                        recuComp = false;
                        while(!recuComp){ 
                            DCrapide->run(RELEASE);//moteur stop necessaire DC
                            Wire.requestFrom(ADRESSE_COMP, 1);
                            recuComp = Wire.read();
                            verifCommande();
                        }
                        DCrapide->run(FORWARD);
                        commSlave(ADRESSE_COMP, deplacement[compteur2][momentEnCours]);     
                        delay(50);                  
                        compteur2++;
                        if(deplacement[compteur2][momentEnCours]==8){
                            Serial.println("derniere pilule a ete captee et envoie de commande de fin"); //print pour les tests
                            delay(1000);
                            DCrapide->run(RELEASE);
                            momentDone = true; 
                            //Attend que cassette soit en place et qu'elle envoie son status
                            recuDist = false;
                            while(!recuDist){ 
                                Wire.requestFrom(ADRESSE_DIST,1);
                                recuDist = Wire.read();
                                verifCommande();
                                //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                                //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier 
                            }
                            Serial.println("cassette prete et en place"); //print pour les tests
                            recuComp = false;
                            while(!recuComp){
                              Wire.requestFrom(ADRESSE_COMP,1);
                              recuComp = Wire.read();
                            } 
                            delay(500);
                            commSlave(ADRESSE_COMP,deplacement[compteur2][momentEnCours]);
                            delay(50);
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
            recuComp = false;
            while(!recuComp){
                Wire.requestFrom(ADRESSE_COMP,1);
                recuComp = Wire.read();
                delay(50);
                verifCommande();
                //verifPil();   //À mettre en commentaires pour les tests sans capteur de purge/pilulier
                //verifPurge(); //À mettre en commentaires pour les tests sans capteur de purge/pilulier
            }
            //while (!capteurCompart) {
            //  verifCompart();
            //}
            Serial.println("tapis compartimentation est pret et en place"); //print pour les tests
            commSlave(ADRESSE_DIST, momentEnCours+1);
            delay(50);
            Serial.println("commande envoyer au slave distribution"); //print pour les tests
            DClent->run(FORWARD);
            DCrapide->run(FORWARD);
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

// Fonctions

// Fonction de communication: Envoie une commande à un des slaves
// Paramètres : 
// address : adresse du slave avec lequel on veut communiquer (int)
// commande : commande qu'on souhaite envoyée au slave (byte)
void commSlave(int address, byte commande){
    Wire.beginTransmission(address);
    Wire.write(commande);
    Wire.endTransmission();
}

// Fonction de purge partielle: vide le tapis rapide
void purgePartielle() {
  DClent->run(RELEASE);
  DCrapide->run(BACKWARD); 
  delay(2000); // Temps à déterminer, uncomment pour vrai code
  DCrapide->run(RELEASE);
}

// Fonction de purge complète: vide le système complet
void purgeComplete() {
    DClent->setSpeed(250); // À déterminer
    DClent->run(FORWARD); // À voir si on a besoin de run le moteur après avoir changer la vitesse
    DCrapide->run(BACKWARD); 
    delay(5000);
    DClent->run(RELEASE);
    DCrapide->run(RELEASE);
    initVar();
}

// Fonction d'arrêt: engendre une purge complète et arrête les moteurs liés aux slaves
void arret(){
    purgeComplete();
    commSlave(ADRESSE_DIST, 0);
    commSlave(ADRESSE_COMP, 0);
    Serial.println("ok");
}

// Fonction pause: arrête tous les moteurs
// Problème: si les slave effectuaient une commande lorsque pause() est appelée
// la commande ne sera pas reprise au point d'arrêt
void pause(){
    DClent->run(RELEASE);
    DCrapide->run(RELEASE);
    commSlave(ADRESSE_DIST, 0);
    commSlave(ADRESSE_COMP, 0);
    while(Serial.available() ==0){
        verifPil();
        verifPurge();
        timePilule = millis();
    }
    verifCommande();
}

//Fonction de vérification: vérifie si l'interface tente de communiquer avec le master
//si c'est le cas, la commande est lue et executée
void verifCommande(){
    if(Serial.available()!=0){
        commande = Serial.read();
        switch (commande) {
        case 2:
            arret();
            break;
        case 3: 
            timePilule = millis();
            Serial.println("Redemarrage reussi");       //print pour les tests
            DCrapide->run(FORWARD);
            DClent->run(FORWARD);
            break;
        case 5:
            pause();
            break;
        default:
            Serial.println("Commande recue inconnue");  //print pour les tests
            arret();
            break;
        }
    }
}
// Fonction de vérification de la purge: détermine si la purge est en place
void verifPurge(){
    if(digitalRead(capPurPin)){
        pause();
        Serial.println("e2");
        Serial.println("Veuillez bien insérer le récipient de purge");
        while(digitalRead(capPurPin)){
            timePilule = millis();
        }
        delay(100);
        DCrapide->run(FORWARD);
        DClent->run(FORWARD);
    }
}

// Fonction de vérification du pilulier: détermine si le pilulier est en place
void verifPil(){
    if(digitalRead(capPilPin)){
        pause();
        Serial.println("e1");
        Serial.println("Veuillez bien insérer le pilulier");
        while(digitalRead(capPilPin)){
            timePilule = millis();
        }
        delay(100);
        DCrapide->run(FORWARD);
        DClent->run(FORWARD);
    }
}

void verifCompart() {
  uint8_t range = sensorBleu.readRange();
  uint8_t status = sensorBleu.readRangeStatus();
  if (status = VL6180X_ERROR_NONE) {
    while (range < DIST_SEUIL3) {
      Wire.beginTransmission(ADRESSE_COMP);
      Wire.write("10");
      Wire.endTransmission();
      delay(50);
    }
    capteurCompart = true;
  }
}

// Fonction d'assignation d'adresse: Assigne une adresse aux capteurs
void setSensorsAddress() {
    // Changer l'addresse du sensor1
    pinMode(SENSOR1_XSHUNT_PIN, INPUT);
    delay(100);
    Serial.println("avant init sensor1");
    while(sensor1.init(true)==0){
        delay(20);
    }
    Serial.println("sensor 1 ready");
    delay(100);
    sensor1.setAddress(SENSOR1_ADDRESS);
    delay(100);
    // Changer l'addresse du sensor2
    pinMode(SENSOR2_XSHUNT_PIN, INPUT);
    delay(100);
    while(sensor2.init(true)==0){
        delay(20);
    }
    Serial.println("sensor 2 ready");
    delay(100);
    sensor2.setAddress(SENSOR2_ADDRESS);
    delay(100);

    // Activer le sensor bleu
//    Serial.println("SensorBleu");
//    pinMode(SENSOR_BLEU_SHUT_PIN, INPUT);
//    delay(50);
//    Serial.println("SensorBleu declare");
//    sensorBleu.begin();
//    delay(50);
}
