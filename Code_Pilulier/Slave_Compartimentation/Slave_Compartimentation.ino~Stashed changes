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
int LED = 13;
int x = 0;
int pos;
void setup() {
	// Define the LED pin as Output
	pinMode(LED, OUTPUT);
	ASstepper2.setMaxSpeed(700);
	ASstepper2.setAcceleration(4000);
	ASstepper2.setSpeed(700);
	// Start the I2C Bus as Slave on address 9
	Wire.begin(10);
	// Attach a function to trigger when something is received.
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
}
void receiveEvent(int bytes) {
	pos = 400 * Wire.read();  // Pour les PIN MS1 et MS2 connectés au ground du EasyDriver, 400 steps = 1 tour
	// Si MS1 et MS2 ne sont pas connectés au ground, 1600 steps = 1 tour
	ASstepper2.setCurrentPosition(0);
	ASstepper2.moveTo(pos);
}

void requestEvent() {
	bool termine = false;
	if (ASstepper2.distanceToGo() == 0) {
		termine = true;
	}
	Wire.write(termine);
}

void loop() {
	ASstepper2.runSpeed();

}
