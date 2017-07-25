#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"

int potPin = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println("test");
    Serial.println(analogRead(potPin));
    delay(1000);
}
