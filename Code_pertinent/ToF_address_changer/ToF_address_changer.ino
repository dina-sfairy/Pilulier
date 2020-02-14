#include <Adafruit_MotorShield.h>

#include <Wire.h>
#include <VL53L0X.h>


int potPin1 = A0; // mettre le 1er potentiometre a la pin A0
int potPin2 = A1; // mettre le 2e potentiometre a la pin A1
int valLue = 0;

Adafruit_MotorShield AFMS;
Adafruit_DCMotor *moteur1;
Adafruit_DCMotor *moteur2;


VL53L0X sensor1;
VL53L0X sensor2;
const int SENSOR1_XSHUNT_PIN = 12;
const int SENSOR2_XSHUNT_PIN = 13;
const uint8_t SENSOR1_ADDRESS = 5;
const uint8_t SENSOR2_ADDRESS = 6;

void setSensorsAddress() {
  // Changer l'addresse du sensor1
  pinMode(SENSOR1_XSHUNT_PIN, INPUT);
  delay(100);
  sensor1.init(true);
  delay(100);
  sensor1.setAddress(SENSOR1_ADDRESS);
  delay(100);
  

  // Changer l'addresse du sensor2
  pinMode(SENSOR2_XSHUNT_PIN, INPUT);
  delay(100);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress(SENSOR2_ADDRESS);
  delay(100);

  

}

void setup() {
  // put your setup code here, to run once:
  pinMode(SENSOR1_XSHUNT_PIN, OUTPUT);
  pinMode(SENSOR2_XSHUNT_PIN, OUTPUT);
  digitalWrite(SENSOR1_XSHUNT_PIN, LOW);
  digitalWrite(SENSOR2_XSHUNT_PIN, LOW);
  Serial.begin(9600);
  Wire.begin();
  Serial.println("test1");
  setSensorsAddress();
  Serial.println("test2");

 AFMS = Adafruit_MotorShield(); 
  AFMS.begin();
  moteur1 = AFMS.getMotor(1);
  moteur2 = AFMS.getMotor(3);

  moteur1->setSpeed(0);
  moteur2->setSpeed(0); 
  moteur1->run(FORWARD);
  moteur2->run(FORWARD);
  Serial.println("test3");
  delay(50);

  //sensor1.setMeasurementTimingBudget(100000);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print(sensor1.readRangeSingleMillimeters());
  //Serial.print(",");
  //Serial.println("test4");
  //Serial.println(sensor2.readRangeSingleMillimeters());
  //Serial.println("test5");


  // Lire la valeur aux potentiometres et ajuster
  // la vitesse des moteurs en consequence.
  valLue = int(float(analogRead(potPin1))*255/1024);
  Serial.println(valLue); // Décommenter si tu veux écrire à l'écran
  moteur1->setSpeed(valLue);
  valLue = int(float(analogRead(potPin2))*255/1024);
  Serial.println(valLue);
  moteur2->setSpeed(valLue);
    delay(50);
    


}
