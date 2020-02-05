#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1;
VL53L0X sensor2;
const int SENSOR1_XSHUNT_PIN = 30;
const int SENSOR2_XSHUNT_PIN = 32;
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
  setSensorsAddress();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(sensor1.readRangeSingleMillimeters());
  Serial.println(sensor2.readRangeSingleMillimeters());

}
