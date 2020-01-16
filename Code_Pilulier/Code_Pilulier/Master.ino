
#include <TimedAction.h>
#include <Wire.h>
int nano_comp = 3;
int nano_dist = 4;

// Test de vérification

TimedAction timedActionComp = TimedAction(4000, commanderCompartimentation);
TimedAction timedActionDist = TimedAction(4000, commanderDistribution);

void setup() {
	// Start the I2C Bus as Master
	Wire.begin();
}

void loop() {
	timedActionComp.check();
	timedActionDist.check();
	delay(100);
	Wire.requestFrom(11, 1);
	bool ready = false;
	while (Wire.available())    // slave may send less than requested
	{
		ready = Wire.read();
	}
	//Serial.println(ready);

}


void commanderCompartimentation() {

	Wire.beginTransmission(10); // transmit to device #10
	Wire.write(nano_comp);              // sends x 
	Wire.endTransmission();    // stop transmitting
}

void commanderDistribution() {
	Wire.beginTransmission(11); // transmit to device #11
	Wire.write(nano_dist);              // sends x 
	Wire.endTransmission();    // stop transmitting
}