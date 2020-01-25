/*
Name:     Lecture_Capteur_Nano.ino
Created:  23/01/2020
Author:   Dina Sfairy
*/

#include <Servo.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <VL53L0X.h>

byte pinMega = 2; // Definir la pin 2 comme input dans le méga; Pins interrupt possibles pour Méga: 2, 3, 18, 19, 20, 21
volatile byte state = LOW;
int seuilDetectionPilule = 50; // À changer après avoir fait des tests; en mm
int distancePill;
byte DIST_REF2;

VL53L0X sensor;

#define HIGH_SPEED

void setup() {
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  
  #if defined HIGH_SPEED
    // reduce timing budget to 50 ms (default is about 33 ms)
    sensor.setMeasurementTimingBudget(50000);
#elif defined HIGH_ACCURACY
    // increase timing budget to 200 ms
    sensor.setMeasurementTimingBudget(200000);
#endif
  pinMode(13, OUTPUT); // set la pin 13 du nano comme output
  DIST_REF2 = sensor.readRangeSingleMillimeters();
}

void loop() {
  distancePill = sensor.readRangeSingleMillimeters();
  Serial.println(distancePill);
  if (distancePill <= seuilDetectionPilule) {
    state = !state;
    digitalWrite(13, state);
    state = LOW;
  }
  else
    digitalWrite(13, state);
}



