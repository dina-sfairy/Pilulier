
#include <TimedAction.h>
#include <Wire.h>
int nano_comp = 1;
int nano_dist = 4;
int test[9] = { 0,1,2,3,8,7,8, 1,8 };
int pos;
// Test de vérification

TimedAction timedActionComp = TimedAction(3500, commanderCompartimentation);
TimedAction timedActionDist = TimedAction(8000, commanderDistribution);

void setup() {
	// Start the I2C Bus as Master
	Wire.begin();
	pinMode(24, INPUT);
	pos = 0;
}

void loop() {
	
	//timedActionComp.check();
	//timedActionDist.check();
	delay(500);
	timedActionComp.check();

	

	//Serial.println(nano_dist);
	//Wire.requestFrom(11, 1);
	//bool ready = false;
	//while (Wire.available())    // slave may send less than requested
	//{
	//	ready = Wire.read();
	//}
	//Serial.println(ready);
	

}


void commanderCompartimentation() {
	/*if (nano_comp > 7) {
		nano_comp = 1;
	}
	else if (nano_comp == 4) {
		nano_comp = 8;
	}*/
	nano_comp = test[pos];
	pos++;
	Serial.println(nano_comp);
	Wire.beginTransmission(11); // transmit to device #11
	Wire.write(nano_comp);              // sends x 
	Wire.endTransmission();    // stop transmitting
	//nano_comp++;
	if (pos > 8) {
		pos = 0;
	}
}

void commanderDistribution() {
	Wire.beginTransmission(11); // transmit to device #11
	Wire.write(nano_dist);              // sends x 
	Wire.endTransmission();    // stop transmitting
}