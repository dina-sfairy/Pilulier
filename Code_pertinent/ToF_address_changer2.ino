#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_VL6180X.h>

VL53L0X sensor1;
VL53L0X sensor2;
Adafruit_VL6180X sensorBleu;
const int SENSOR_BLEU_SHUT_PIN = 11;
const int SENSOR1_XSHUNT_PIN = 12;
const int SENSOR2_XSHUNT_PIN = 13;
const uint8_t SENSOR1_ADDRESS = 5;
const uint8_t SENSOR2_ADDRESS = 6;

void setSensorsAddress() {
  // Changer l'addresse du sensor1
  Serial.println("Sensor1");
  pinMode(SENSOR1_XSHUNT_PIN, INPUT);
  delay(50);
  Serial.println("before init");
  while (sensor1.init(true) == 0) {
    delay(50);
    Serial.println("retrying");
  }
  delay(50);
  sensor1.setAddress(SENSOR1_ADDRESS);
  delay(50);

  // Changer l'addresse du sensor2
  Serial.println("Sensor2");
  pinMode(SENSOR2_XSHUNT_PIN, INPUT);
  delay(50);
  while (sensor2.init(true) == 0) {
    delay(50);
    Serial.println("retrying");
  }
  delay(50);
  sensor2.setAddress(SENSOR2_ADDRESS);
  delay(50);
  Serial.println("Senseur verts changés.");

  // Activer le sensor bleu
    Serial.println("SensorBleu");
    pinMode(SENSOR_BLEU_SHUT_PIN, INPUT);
    delay(50);
    sensorBleu = Adafruit_VL6180X();
    delay(50);
    Serial.println("SensorBleu declare");
    sensorBleu.begin();
    delay(50);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SENSOR1_XSHUNT_PIN, OUTPUT);
  pinMode(SENSOR2_XSHUNT_PIN, OUTPUT);
  pinMode(SENSOR_BLEU_SHUT_PIN, OUTPUT);
  digitalWrite(SENSOR1_XSHUNT_PIN, LOW);
  digitalWrite(SENSOR2_XSHUNT_PIN, LOW);
  digitalWrite(SENSOR_BLEU_SHUT_PIN, LOW);
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Start");
  setSensorsAddress();
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print(sensor1.readRangeSingleMillimeters());
    Serial.print(",");
    Serial.print(sensor2.readRangeSingleMillimeters());
    Serial.print(",");

//  Serial.println(sensor1.readRangeSingleMillimeters());
//  Serial.println(sensor2.readRangeSingleMillimeters());

  float lux = sensorBleu.readLux(VL6180X_ALS_GAIN_5);
  uint8_t range = sensorBleu.readRange();
  uint8_t status = sensorBleu.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) {
    Serial.println(range);
  } else {
    Serial.println(0);
  }
  //delay(100);


}
