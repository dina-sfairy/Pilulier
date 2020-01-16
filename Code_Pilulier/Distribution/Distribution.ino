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
int LED = 13;
int x = 0;
int pos = 0;
int momentJournee = 0;
bool retour;
int etape;
int servoPin = 3;
// Create a servo object 
Servo Servo1;

void setup() {
	// Define the LED pin as Output
	pinMode(LED, OUTPUT);
	ASstepper2.setMaxSpeed(6000);
	ASstepper2.setAcceleration(4000);
	// Start the I2C Bus as Slave on address 11
	Wire.begin(11);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	retour = false;
	etape = 1;
	Servo1.attach(servoPin);
	Servo1.write(0);
	delay(500);

}
void receiveEvent(int bytes) {
	//pos = 1600 * Wire.read();  // Pour les PIN MS1 et MS2 connectés au ground du EasyDriver, 400 steps = 1 tour
	momentJournee = Wire.read();
	retour = false;
	etape = 1;
	switch (momentJournee)
	{
	case 1:
		pos = 0;
		break;
	case 2:
		pos = (int)(20 * 57 / PI); // Les calculs assument 1 tour = 400 steps
		break;
	case 3:
		pos = (int)(20 * (57 + 63) / PI);
		break;
	case 4:
		pos = (int)(20 * (57 * 2 + 63) / PI);
		break;
	default:
		break;
	}
	// Si MS1 et MS2 ne sont pas connectés au ground, 1600 steps = 1 tour
	Serial.println(momentJournee);
	ASstepper2.setCurrentPosition(0);
	ASstepper2.moveTo(pos);

}

void requestEvent() {
	digitalWrite(LED, HIGH);
	delay(200);
	digitalWrite(LED, LOW);
	//Serial.print("Request event : "); Serial.println(termine);
	Wire.write(retour);

}

void loop() {
	ASstepper2.run();

	if (ASstepper2.distanceToGo() == 0) {

		if (etape == 1) {

			Servo1.write(90);
			delay(500);
			Servo1.write(0);
			delay(500);
			pos = -pos;
			ASstepper2.setCurrentPosition(0);
			ASstepper2.moveTo(pos);
			etape++;
		}
		else if (etape == 2) {
			retour = true;
		}


	}

}
