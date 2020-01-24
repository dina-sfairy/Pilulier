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
int seuilDetectionPilule = 15; // À changer après avoir fait des tests; en mm
int distancePill;
byte DIST_REF2;

VL53L0X sensor;

void setup() {
  pinMode(13, OUTPUT); // set la pin 13 du nano comme output
  DIST_REF2 = sensor.readRangeSingleMillimeters();
}

void loop() {
  distancePill = sensor.readRangeSingleMillimeters();
  if (DIST_REF2 - distancePill <= seuilDetectionPilule) {
    state = !state;
    digitalWrite(13, state);
    state = LOW;
  }
  else
    digitalWrite(13, state);
}

/* À écrire dans le MÉGA
 *  const byte pinInterruptSensor = 2;
 * void setup() {
 * ...
 * attachInterrupt(digitalPinToInterrupt(pinInterruptSensor), piluleDectecteeSep, RISING);
 * ...
 * } 
 * 
 * void piluleDetecteeSep() {
 * Wire.beginTransmission(11);                          // transmit to device #11
 * Wire.write(deplacement[momentEnCours][compteur2]);   // sends x 
 * Wire.endTransmission();                              // stop transmitting
 * }

*/

