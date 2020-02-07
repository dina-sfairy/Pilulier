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
int LED = 13;
int pos;
int momentJournee;
bool retour;
int etape;
int servoPin;
int homePosition;
int posAjustement;
int nbSteps;
int compteurAjustement;

// Create a servo object 
Servo Servo1;

void setup() {
	// Define the LED pin as Output
	pinMode(LED, OUTPUT);
	ASstepper2.setMaxSpeed(600);
	ASstepper2.setAcceleration(300);
	ASstepper2.setCurrentPosition(0);
	// Start the I2C Bus as Slave on address 11
	Wire.begin(11);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	retour = false; 
	etape = 2;
	pos = 1;
	momentJournee = 0;
	servoPin = 12;
	nbSteps = 0;
	posAjustement = (int)(100 / PI);
	compteurAjustement = 0;
	Servo1.attach(servoPin);
	Servo1.write(163); // À ajuster en fonction de la position réelle pour que la trappe soit horizontale initialement
	delay(500);
	pinMode(6, INPUT);

}
void receiveEvent(int bytes) {
	nbSteps = 0;
	compteurAjustement = 0;
	//pos = 1600 * Wire.read();  // Pour les PIN MS1 et MS2 connectés au ground du EasyDriver, 400 steps = 1 tour
	momentJournee = Wire.read();
	Serial.println(momentJournee);
	etape = 1;
	retour = true;
	// pos = 0;

	switch (momentJournee)
	{
	case 1:
		pos = (int)((2*90/25)*100/ PI); // Les calculs assument 1 tour = 400 steps
		retour = false;
		break;
	case 2:
		pos = (int)((2*(90 + 55)/25)*100 / PI); // Les calculs assument 1 tour = 400 steps
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
		retour = true; // À cause des problèmes de communication, on ne veut pas entrer dasn la première boucle
		break;
	}
	// Si MS1 et MS2 ne sont pas connectés au ground, 1600 steps = 1 tour
	Serial.println(momentJournee);
	Serial.println(pos);
	if (pos > 0) {
		ASstepper2.setCurrentPosition(0);
		ASstepper2.moveTo(-pos);
	}
	else {
		Serial.println("Position 0");
	}
	

}

void requestEvent() {
	
	Wire.write(retour);
}

void loop() {

	ASstepper2.run();
	nbSteps++;
	if (ASstepper2.distanceToGo() == 0 && !retour && nbSteps == pos && etape == 1) {
		Servo1.write(Servo1.read() - 60);
		delay(500);
		Servo1.write(Servo1.read() + 60);
		delay(500);
		ASstepper2.moveTo(0);
		etape++;
		nbSteps = 0;
	}
	else if (ASstepper2.distanceToGo() == 0 && etape == 2) {
		if (digitalRead(11) != 0) {
			compteurAjustement++;
			ASstepper2.moveTo(compteurAjustement*posAjustement);
			nbSteps = 0;
		}
		else
		{
			retour = true;
			etape = 1;
			momentJournee = 0;
			pos = 0;
			nbSteps = 0;
		}
	}

	


	// Première version du code semi-problématique
	/*
	ASstepper2.run();
	if (ASstepper2.distanceToGo() == 0) {

		if (etape == 1 && !retour ) {

			Servo1.write(Servo1.read() - 60);
			delay(500);
			Servo1.write(Servo1.read() + 60);
			delay(500);
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(-abs(pos));
			pos = 0;
			etape++;
		}
		else if (etape == 2) {
			if (digitalRead(6) != 0) {
				ASstepper2.setCurrentPosition(0);
				ASstepper2.moveTo(-posAjustement);
			}
			else {
				retour = true;
				etape = 1;
				momentJournee = 0;
				pos = 0;
			}
		}


	}

	*/

}
