/*
 Name:		Test_Slave_UNO.ino
 Created:	04/01/2020 13:28:34
 Author:	jonat
*/




#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>

AccelStepper ASstepper2(AccelStepper::DRIVER, 2, 3);
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
int servoPin = 12;
int posAjustement = (int)(22/PI); // Ajustement de 5mm lin�aire
bool ouvertureFermeture;
bool ajustement;
// Create a servo object 
Servo Servo1;

int etat; // Variable d�terminant l'�tat de compl�tion d'une prescription par le tapis de compartimentation. Cette variable est renvoy�e au Master sur demande.

void setup() {
	// Define the LED pin as Output
	ASstepper2.setMaxSpeed(50); // 200 et 400 acc
	ASstepper2.setAcceleration(50);
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
	Serial.println(Servo1.read());
	Servo1.write(180);
	//Servo1.write(0); // � ajuster en fonction de la position r�elle pour que la trappe soit horizontale initialement
	delay(500);
}
void receiveEvent(int bytes) {
	jour = Wire.read();
	Serial.println(jour);
	pos = 0;
	pret = false;
	finPrescription = false;
	ouvertureFermeture = false;
	calibration = false;
	ajustement = false;
	switch (jour)
	{
	case 0:
		pos = (int) (jour*190/PI); // 190/PI steps pour d�placer lat�ralement de 45mm lorsque MS! et MS2 sont au ground
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
		if (posTetePrescription < 7)
		{
			ajustement = true;
		}
		else
			ajustement = false;

	case 9:
		pos = posAjustement;
		break;

	default:
		pos = 0;
		break;
	}

	
	// Si MS1 et MS2 ne sont pas connect�s au ground, 1600 steps = 1 tour
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
		pret = false;
		if (posTetePrescription == 7) {
			pret = false;
			Servo1.write(Servo1.read() - 90);
			delay(500);
			Servo1.write(Servo1.read() + 90);
			delay(500);
			posTetePrescription = 0;
			pret = true;

		}

		else if (posTetePrescription < 7 && ajustement) {
			ajustement = false;
			pos = (int)((7 - posTetePrescription) * 190 / PI);
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(pos);
			posTetePrescription = 7;
			finPrescription = true;
		}
		else
			posTetePrescription = 0;

		
	}
	
	if (calibration && ASstepper2.distanceToGo() == 0 && !pret) {
		if (digitalRead(6) != 0) { // � modifier selon le type de capteur requis. Ici on utilise un interrupteur
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
