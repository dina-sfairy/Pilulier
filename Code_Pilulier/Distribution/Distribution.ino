/*
 Name:		Distribution.ino
 Created:	04/01/2020 13:28:34
 Author:	Jonathan Lacombe
 Description : Code de routine pour le microcontr�leur esclave commandant le sous-syst�me de distribution

*/



// Librairies
#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>


// D�claration des variables
AccelStepper ASstepper2(AccelStepper::DRIVER, 2, 3);
void ActiverStepper2() {
	ASstepper2.run();
} 
int pos; // Position � atteindre
int momentJournee; // 1,2,3,4 pour matin-midi-soir-nuit
bool retour; // Retour � la position d'attente sous le tapis de compartimentation
int etape; // 1 ou 2 pour aller ou retour de la cassette
int servoPin; // Pin de contr�le du servomoteur
int posAjustement; // Ajustement de 5mm lin�aire lorsque la cassette n'est pas � sa position de r�f�rence
int nbSteps; // Nombre de pas effectu�s
int compteurAjustement; // Nombre d'ajustements de 5mm � effectuer
Servo Servo1;

// Si MS1 et MS2 (EasyDriver) ne sont pas connect�s au ground, 1600 steps = 1 tour. Sinon, 200 steps = 1 tour
void setup() {
	
	ASstepper2.setMaxSpeed(400); // step/seconde
	ASstepper2.setAcceleration(200); //step/seconde^2
	ASstepper2.setCurrentPosition(0); 
	// D�marre le bus I2C pour l'esclave � l'adresse 11
	Wire.begin(11);
	// Fonction d�clench�es lorsqu'un �v�nement se produit
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);

	// Initialisation des variables
	retour = false; 
	etape = 2; // Force l'ajustement de la position de la cassette automatiquement si celle-ci n'est pas � sa position d'attente
	pos = 1;
	momentJournee = 0;
	servoPin = 12;
	nbSteps = 0;
	posAjustement = (int)(100 / PI); // Ajustement de 5mm lin�aire lorsque la cassette n'est pas � sa position de r�f�rence
	compteurAjustement = 0;
	Servo1.attach(servoPin);
	Servo1.write(163); // Position de la trappe lorsqu'elle est ferm�e
	delay(500);
	pinMode(6, INPUT); // Interrupteur m�canique qui d�tecte la pr�sence/absence de la cassette � la position d'attente

}

// Lorsque le Master envoie un byte (par construction, un chiffre de 1 � 4)
void receiveEvent(int bytes) {
	nbSteps = 0;
	compteurAjustement = 0;
	momentJournee = Wire.read();
	// Serial.println(momentJournee);
	etape = 1;
	retour = true;

	switch (momentJournee)
	{
	case 1:
		pos = (int)((2*90/25)*100/ PI); // Les calculs assument 1 tour = 400 steps. Correspond � la position angulaire � atteindre 
		// afin de se positionner vis-�-vis le premier moment de la journ�e
		retour = false;
		break;
	case 2:
		pos = (int)((2*(90 + 55)/25)*100 / PI); 
		retour = false;
		break;
	case 3:
		pos = (int)((2 * (90 + 55 + 60) / 25) * 100 / PI);
		retour = false;
		break;
	case 4:
		pos = (int)((2 * (90 + 55*2 + 60) / 25) * 100 / PI);
		retour = false;
		break;
	default:
		retour = true; // � cause des probl�mes de communication potentiels, on ne veut pas entrer dans la premi�re boucle
		break;
	}
	//Serial.println(momentJournee);
	//Serial.println(pos);
	if (pos > 0) {
		ASstepper2.setCurrentPosition(0);
		ASstepper2.moveTo(-pos); // � cause de l'orientation du moteur pas-�-pas et de l'attachement de la courroie � la cassette, rotation anti-horaire pour avancer la cassette
	}
	else {
		//Serial.println("Position 0");
	}
	

}

// Lorsque le Master demande un byte (bool�en pour informer de l'�tat de la cassette de distribution, c'est-�-dire si elle a atteint sa position d'attente sous le tapis de compartimentation)
void requestEvent() {
	
	Wire.write(retour);
}


// Boucle principale
void loop() {


	// Si une incr�mentation de pas (step) est due, d�clenche un step
	ASstepper2.run();
	nbSteps++;

	// La cassette est arriv�e vis-�-vis le moment de la journ�e cible
	if (ASstepper2.distanceToGo() == 0 && !retour && nbSteps == pos && etape == 1) { 
		// Ouverture/fermeture de la trappe
		Servo1.write(Servo1.read() - 60); 
		delay(500);
		Servo1.write(Servo1.read() + 60);
		delay(500);
		ASstepper2.moveTo(0); // Position d'attente
		etape++; // On actualise l'�tape pour passer � la phase de retour
		nbSteps = 0;
	}
	// La cassette est arriv�e � la position d'attente
	else if (ASstepper2.distanceToGo() == 0 && etape == 2) {
		if (digitalRead(11) != 0) { // La cassette n'a pas enclench� l'interrupteur m�canique
			compteurAjustement++;
			ASstepper2.moveTo(compteurAjustement*posAjustement);
			nbSteps = 0;
		}
		else
		{
			retour = true; // La cassette est sous le tapis de compartimentation et pr�te � recevoir d'autres pilules
			etape = 1;
			momentJournee = 0;
			pos = 0;
			nbSteps = 0;
		}
	}


}
