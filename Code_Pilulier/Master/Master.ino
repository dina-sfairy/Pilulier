
#include <TimedAction.h>
#include <Wire.h>

int nano_comp = 1;
int nano_dist = 1;
int test[7] = { 1,0,0,2,1,1,0 };
int test_dist[4] = { 1,2,3,4 };
int pos;
bool ready;
int presentTime = 0;
int chiffre;
// Test de vérification
TimedAction timedActionComp = TimedAction(2000, commanderCompartimentation);
TimedAction timedActionDist = TimedAction(8000, commanderDistribution);

int vect[10];
const int size = sizeof(vect);
int tab[size];


void setup() {
	// Start the I2C Bus as Master
	Wire.begin();
	pinMode(24, INPUT);
	pos = 0;
	presentTime = millis();
	ready = false;
	chiffre = 0;
	
}

void loop() {
	
	//timedActionComp.check();
	timedActionDist.check();
	
	

	//if (millis() - presentTime > 2000)
	//{
	//	presentTime = millis();
	//	Wire.requestFrom(11, 1);
	//	ready = true;
	//	while (Wire.available())    // slave may send less than requested
	//	{
	//		chiffre = Wire.read();
	//	}
	//	Serial.println(chiffre);
	//	if (chiffre) {
	//		Wire.beginTransmission(11);
	//		Wire.write(nano_dist);
	//		Wire.endTransmission();
	//	}
	//}
	

}


void commanderCompartimentation() {
	
	
	nano_comp = test[pos];
	pos++;
	Serial.println(nano_comp);
	Wire.beginTransmission(10); // transmit to device #11
	Wire.write(nano_comp);              // sends x 
	Wire.endTransmission();    // stop transmitting
	//nano_comp++;
	if (pos == 7) {
		pos = 0;
		ready = false;
		Wire.requestFrom(10, 1);
		while (Wire.available())
		{
			ready = Wire.read();
			Serial.print("Slave pret : "); Serial.println(ready);
		}
		if (ready) {
			Wire.beginTransmission(10); // transmit to device #10
			Wire.write(8);              // sends x 
			Wire.endTransmission();
		}
	}
	
	

	//Wire.beginTransmission(11); // transmit to device #11
	//Wire.write(nano_comp);              // sends x 
	//Wire.endTransmission();    // stop transmitting

}

void commanderDistribution() {
	//Serial.println(nano_dist);
	//Wire.beginTransmission(11); // transmit to device #11
	//Wire.write(nano_dist);              // sends x 
	//Wire.endTransmission();    // stop transmitting

	nano_dist = test_dist[pos];
	pos++;
	Wire.beginTransmission(11); // transmit to device #11
	Wire.write(nano_dist);              // sends x 
	Wire.endTransmission();    // stop transmitting
	if (pos > 3) {
		pos = 0;
	}
}