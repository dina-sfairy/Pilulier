/*
 Name:		Test_Slave_UNO.ino
 Created:	04/01/2020 13:28:34
 Author:	jonat
*/




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
void setup() {
	// Define the LED pin as Output
	ASstepper2.setMaxSpeed(4000);
	//ASstepper2.setAcceleration(6000);
	// Start the I2C Bus as Slave on address 10
	Wire.begin(10);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	calibration = false;
	jour = 0;
}
void receiveEvent(int bytes) {
	jour = Wire.read();
	pos = 0;
	termine = false;
	switch (jour)
	{
	case 0:
		pos = (int) (jour*400/PI);
		calibration = true;
		break;
	case 1:
		pos = (int)(jour * 400 / PI);
		break;
	case 2:
		pos = (int)(jour * 400 / PI);
		break;
	case 3:
		pos = (int)(jour * 400 / PI);
		break;
	case 4:
		pos = (int)(jour * 400 / PI);
		break;
	case 5:
		pos = (int)(jour * 400 / PI);
		break;
	case 6:
		pos = (int)(jour * 400 / PI);
		break;
	case 7:
		pos = (int)(jour * 400 / PI);
		break;
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

	if (ASstepper2.distanceToGo() == 0) {
		termine = true;
	}


}
