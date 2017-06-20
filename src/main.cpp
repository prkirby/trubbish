/**
* Blink
*
* Turns on an LED on for one second,
* then off for one second, repeatedly.
*/
#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include "smoother/smoother.h"
#include "servo/servo.h"

// Servo Drivers
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//Servos
Servo mainServo = Servo(pwm, 0, 800, 3500);
Servo secondaryServo = Servo(pwm, 1, 800, 3500);
Servo thirdServo = Servo(pwm, 2, 800, 3500);

// Sonar
int trigPin = 12;
int echoPin = 13;
unsigned int maxDistance = 80;
unsigned long prevDistance = 0;

// Smoothing modules
Smoother<unsigned int, 20> distanceSmoother;

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");

  pwm.begin();          // Start that bitch up
  pwm.setPWMFreq(333);  // Set frequency for servo drivers

  pinMode(trigPin, OUTPUT); // Set up sonar pins
  pinMode(echoPin, INPUT);

  yield();
}

/**
 * Get distance from the sonar
 * @return distance in cm
 */
unsigned int getDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned int duration = pulseIn(echoPin, HIGH);
  unsigned int distance = duration / 2 / 29.1;
  delay(1);
  return distance;
}

void loop() {
  // Get distance
  unsigned int distance = getDistance();
  // If its too big break
  if (distance > maxDistance ) return;
  // If its the same as last time, break
  if (prevDistance == distance) return;

  prevDistance = distance;

  // Smooth that bitch out
  distance = distanceSmoother.smooth(distance);

  mainServo.move(map(distance, 3, maxDistance, 0, 100));
  secondaryServo.move(map(distance, 3, maxDistance, 100, 0));
  thirdServo.move(map(distance, 3, maxDistance, 30, 80));

  delay(3);
}
