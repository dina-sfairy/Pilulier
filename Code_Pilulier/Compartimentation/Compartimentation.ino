/*
 Name:		Test_Slave_UNO.ino
 Created:	04/01/2020 13:28:34
 Author:	jonat
*/




#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>

AccelStepper ASstepper2(AccelStepper::DRIVER, 9, 8);
void ActiverStepper2() {
	ASstepper2.run();
}
int x = 0;
int pos;
int jour;
int nbSteps;
bool calibration;
bool pret;
bool finPrescription;
int posTetePrescription;
int servoPin = 3;
int posAjustement = (int)(800 / (18*PI)); // Ajustement de 5mm linéaire
bool ouvertureFermeture;
bool ajustement;
// Create a servo object 
Servo Servo1;

int etat; // Variable déterminant l'état de complétion d'une prescription par le tapis de compartimentation. Cette variable est renvoyée au Master sur demande.

void setup() {
	// Define the LED pin as Output
	ASstepper2.setMaxSpeed(200); // 200 et 400 acc
	ASstepper2.setAcceleration(100);
	// Start the I2C Bus as Slave on address 10
	Wire.begin(10);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	calibration = true;
	ajustement = true;
	jour = 0;
	posTetePrescription = 0;
	finPrescription = false;
	ouvertureFermeture = false;
	pinMode(6, INPUT);
	nbSteps = 0;
	Servo1.attach(servoPin);
	Servo1.write(0); // À ajuster en fonction de la position réelle pour que la trappe soit horizontale initialement
	delay(500);
}
void receiveEvent(int bytes) {
	jour = Wire.read();
	pos = 0;
	pret = false;
	finPrescription = false;
	ouvertureFermeture = false;
	calibration = false;
	ajustement = false;
	switch (jour)
	{
	case 0:
		pos = (int) (jour*190/PI); // 190/PI steps pour déplacer latéralement de 45mm lorsque MS! et MS2 sont au ground
		break;
	case 1:
		pos = (int)(jour * 190 / PI);
		posTetePrescription += 1;
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
	case 8 :
		Serial.println(posTetePrescription);
		pos = 0;
		nbSteps = 0;
		finPrescription = true;
		ouvertureFermeture = true;
		ajustement = true;


	default:
		pos = 0;
		break;
	}

	
	// Si MS1 et MS2 ne sont pas connectés au ground, 1600 steps = 1 tour
	ASstepper2.setCurrentPosition(0);
	ASstepper2.moveTo(pos);
}

void requestEvent() {
	
	Wire.write(pret);
}

void loop() {

	ASstepper2.run();
	if (ASstepper2.distanceToGo() == 0 && !calibration) {
		pret = true;
	}
	if (finPrescription && ASstepper2.distanceToGo() == 0) {
		if (posTetePrescription == 7) {
			Servo1.write(90);
			delay(500);
			Servo1.write(0);
			delay(500);
			posTetePrescription = 0;
		}

		else if (posTetePrescription < 7 && ajustement) {
			ajustement = false;
			pos = (int)((7 - posTetePrescription) * 190 / PI);
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(pos);
			posTetePrescription = 7;
			goto label; // On ne veut pas faire la calibration avant d'avoir ajusté la position de la tête de prescription, donc on passe au-dessu de la première étape pour la première itération
		}
	
		
		// Vérification de la calibration de la case Init. Si le capteur ne détecte rien, on incrémente le tapis de 5mm
		if (digitalRead(6) != 0) { // À modifier selon le type de capteur requis. Ici on utilise un interrupteur
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(posAjustement);
		}
		else {
			finPrescription = false;
			pret = true;
			posTetePrescription = 0;
		}		
	}
	
	if (calibration && ASstepper2.distanceToGo() == 0 && !pret) {
		if (digitalRead(6) != 0) { // À modifier selon le type de capteur requis. Ici on utilise un interrupteur
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(posAjustement);
		}
		else {
			pret = true;
			calibration = false;
			posTetePrescription = 0;
		}
	}
	label:;


}
