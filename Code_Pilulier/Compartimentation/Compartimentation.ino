/*
 Name:		Compartimentation.ino
 Created:	04/01/2020 13:28:34
 Author:	Jonathan Lacombe
 Description : Code de routine pour le microcontrôleur esclave commandant le sous-système de compartimentation
*/



// Librairies
#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>


// Déclaration des variables
AccelStepper ASstepper2(AccelStepper::DRIVER, 2, 3);
void ActiverStepper2() {
	ASstepper2.run();
}
int pos; // Position angulaire à atteindre pour le moteur pas-à-pas
int jour; // 1 à 7 pour les 7 jours de la semaine
int nbSteps;
bool calibration; 
bool pret; // Prêt à recevoir une autre position cible à atteindre
bool finPrescription; // Si la fin de la prescription est atteinte pour ce moment de la journée
int posTetePrescription; // Position du premier boîtier de compartimentation sur les 7 jours de la semaine
int servoPin = 12; // Port d'attache de la pin de contrôle du servomoteur
int posAjustement = (int)(22/PI); // Ajustement de 5mm linéaire lorsque le tapis n'est pas à sa position de référence
bool ouvertureFermeture; // Ouverture/fermeture de la trappe de compartimentation
bool ajustement; // Ajustement linéaire nécessaire
Servo Servo1; // Servomoteur de la trappe
int etat; // Variable déterminant l'état de complétion d'une prescription par le tapis de compartimentation. Cette variable est renvoyée au Master sur demande.


// Si MS1 et MS2 (EasyDriver) ne sont pas connectés au ground, 1600 steps = 1 tour. Sinon, 400 steps = 1 tour
void setup() {
	ASstepper2.setMaxSpeed(100); // step/seconde
	ASstepper2.setAcceleration(150); //step/seconde^2

	// Démarre le bus I2C pour l'esclave à l'adresse 10
	Wire.begin(10);
	// Fonction déclenchées lorsqu'un événement se produit
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);

	// Initialisation des variables
	calibration = true;
	ajustement = true;
	jour = 0;
	posTetePrescription = 0;
	finPrescription = false;
	ouvertureFermeture = false;
	pinMode(6, INPUT); // Capteur de contact lors du prototypage pour l'ajustement linéaire du tapis
	nbSteps = 0;
	Servo1.attach(servoPin);
	Serial.println(Servo1.read());
	Servo1.write(180); // Trappe fermée initialement
	delay(500);
}

// Lorsque le Master envoie un byte (par construction, un chiffre de 0 à 10)
void receiveEvent(int bytes) {
	jour = Wire.read();
	// Serial.println(jour);

	// Initialisation des variables
	pos = 0;
	pret = false;
	finPrescription = false;
	ouvertureFermeture = false;
	calibration = false;
	ajustement = false;
	switch (jour)
	{
	case 0:
		pos = (int) (jour*190/PI); // 190/PI steps pour déplacer latéralement de 45mm lorsque MS! et MS2 sont au ground (les cases du pilulier sont espacées d'environ 45mm)
		break;
	case 1:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 1; // Incrémentation de la position de la tête de la prescription
		break;
	case 2:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 2;
		break;
	case 3:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 3;
		break;
	case 4:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 4;
		break;
	case 5:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 5;
		break;
	case 6:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 6;
		break;
	case 7:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 7;
		break;
	case 8 : // Fin de la prescription pour ce moment de la journée, prêt à ouvrir/fermer la trappe
		// Serial.println(posTetePrescription);
		pos = 0;
		nbSteps = 0;
		finPrescription = true;
		ouvertureFermeture = true;
		if (posTetePrescription < 7)
		{
			ajustement = true; // On doit positionner la tête de prescription vis-à-vis la dernière journée de la semaine
		}
		else
			ajustement = false;
		break;

	case 9: // Incrémenter la position des boîtiers de 5mm pour la calibration
		pos = posAjustement;
		break;

	case 10: // Réinitialisation de la position de la tête de prescription
		posTetePrescription = 0;
		break;

	default:
		pos = 0;
		break;
	}

	// Définition de la position angulaire cible à atteindre au moteur pas-à-pas
	ASstepper2.setCurrentPosition(0);
	ASstepper2.moveTo(pos);
}

// Lorsque le Master demande un byte (booléen pour informer de l'état du tapis de compartimentation, c'est-à-dire s'il a atteint sa position cible)
void requestEvent() {
	
	Wire.write(pret);

}


// Boucle principale
void loop() {

	// Si une incrémentation de pas (step) est due, déclenche un step
	ASstepper2.run();

	// Le tapis a atteint sa position cible, mais ce n'est pas encore la fin de la prescription pour ce moment de la journée
	if (ASstepper2.distanceToGo() == 0 && !calibration) {
		pret = true;
	}

	// Le tapis a atteint sa position cible, et c'est la fin de la prescription pour ce moment de la journée
	if (finPrescription && ASstepper2.distanceToGo() == 0) {
		pret = false;
		if (posTetePrescription == 7) { // Ouverture/fermeture de la trappe
			pret = false;
			Servo1.write(Servo1.read() - 90);
			delay(500);
			Servo1.write(Servo1.read() + 90);
			delay(500);
			posTetePrescription = 0;
			pret = true;

		}

		else if (posTetePrescription < 7 && ajustement) { // Ajustement de la tête de prescription avant ouverture/fermeture de la trappe
			ajustement = false;
			pos = (int)((7 - posTetePrescription) * 190 / PI);
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(pos);
			posTetePrescription = 7; // La tête de prescription sera vis-à-vis la dernière journée de la semaine
			finPrescription = true;
		}
		else
			posTetePrescription = 0;

		
	}
	
	// Ajustement de 5mm nécessaire jusqu'au déclenchement du capteur
	if (calibration && ASstepper2.distanceToGo() == 0 && !pret) {
		if (digitalRead(6) != 0) { // À modifier selon le type de capteur requis. Ici on utilise un interrupteur pour déclencher la calibration de la position du tapis de compartimentation
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(posAjustement);
		}
		else {
			pret = true;
			calibration = false;
			posTetePrescription = 0;
		}
	}


}
