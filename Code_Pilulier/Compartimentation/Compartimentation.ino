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
bool calibration;
bool termine;
bool finPrescription;
int posTetePrescription;
int servoPin = 3;
int posAjustement = (int)(800 / (18*PI)); // Ajustement de 5mm linéaire
bool ouvertureFermeture;
// Create a servo object 
Servo Servo1;

int etat; // Variable déterminant l'état de complétion d'une prescription par le tapis de compartimentation. Cette variable est renvoyée au Master sur demande.

void setup() {
	// Define the LED pin as Output
	ASstepper2.setMaxSpeed(4000);
	ASstepper2.setAcceleration(8000);
	// Start the I2C Bus as Slave on address 10
	Wire.begin(11);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	calibration = false;
	jour = 0;
	posTetePrescription = 0;
	finPrescription = false;
	ouvertureFermeture = false;
	pinMode(6, INPUT);

	Servo1.attach(servoPin);
	Servo1.write(0); // À ajuster en fonction de la position réelle pour que la trappe soit horizontale initialement
	delay(500);
}
void receiveEvent(int bytes) {
	jour = Wire.read();
	pos = 0;
	termine = false;
	finPrescription = false;
	ouvertureFermeture = false;
	calibration = false;
	switch (jour)
	{
	case 0:
		pos = (int) (jour*400/PI);
		calibration = true;
		break;
	case 1:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 2:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 3:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 4:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 5:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 6:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 7:
		pos = (int)(jour * 400 / PI);
		posTetePrescription += jour;
		break;
	case 8 :
		pos = 0;
		finPrescription = true;
		ouvertureFermeture = true;


	default:
		pos = 0;
		break;
	}
	// Si MS1 et MS2 ne sont pas connectés au ground, 1600 steps = 1 tour
	ASstepper2.setCurrentPosition(0);
	ASstepper2.moveTo(pos);
}

void requestEvent() {
	
	Wire.write(termine);
}

void loop() {

	ASstepper2.run();

	if (finPrescription && ASstepper2.distanceToGo() == 0) {

		if (posTetePrescription == 7) {
			Servo1.write(90);
			delay(500);
			Servo1.write(0);
			delay(500);
			posTetePrescription++;
		}

		else if (posTetePrescription < 7) {
			pos = (int)((7 - posTetePrescription) * 400 / PI);
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
			termine = true;
			posTetePrescription = 0;
		}		
	}
	if (calibration && ASstepper2.distanceToGo() == 0 && !termine) {
		if (digitalRead(6) != 0) { // À modifier selon le type de capteur requis. Ici on utilise un interrupteur
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(posAjustement);
		}
		else {
			termine = true;
			calibration = false;
			posTetePrescription = 0;
		}
	}
	label:;


}
