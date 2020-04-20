/*
	--------- Code de prototypage, ne fait pas partie de la solution finale --------------------
*/



#include <TimedAction.h>
#include <Wire.h>

int nano_comp = 1;
int nano_dist = 1;
//int test[7] = { 1,0,0,2,1,1,0 };
int test[7] = { 5,5,5,5,5,5,5 };

int test_dist[4] = { 1,2,3,4 };
int pos;
bool ready;
int presentTime = 0;
int chiffre;
// Test de vérification
TimedAction timedActionComp = TimedAction(7000, commanderCompartimentation);
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
	Wire.beginTransmission(10); 
	Wire.write(10); 
	Wire.endTransmission();
	
}

void loop() {
	
	timedActionComp.check();
	//timedActionDist.check();
	
	

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
	delay(50);
	//nano_comp++;
	if (pos == 7) {
		delay(1000);
		ready = false;
		Wire.requestFrom(10, 2);
		//delay(2000);
		while (Wire.available())
		{
			pos = 0;
			ready = Wire.read();
			Serial.print("Slave pret : "); Serial.println(ready);
			if (ready) {
				Wire.beginTransmission(10); // transmit to device #10
				Wire.write(8);              // sends x 
				Wire.endTransmission();
				delay(2000);
				//Serial.println(8);
				ready = false;
				break;
			}

		}
		Wire.beginTransmission(10);
		Wire.write(10);
		Wire.endTransmission();
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