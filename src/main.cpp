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
#include "animation/animation.h"

// Timing
unsigned long curMillis, prevMillis;

// Sonar
int trigPin = 12;
int echoPin = 13;
unsigned int maxDistance = 80;
unsigned long prevDistance = 0;

// Smoothing modules
Smoother<unsigned int, 20> distanceSmoother;

// Servo Drivers
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servos
Servo mainServo = Servo(pwm, 0, 800, 3500);
Servo secondaryServo = Servo(pwm, 1, 800, 3500);
Servo thirdServo = Servo(pwm, 2, 800, 3500);

//Animation init
Action mainAnimationActions[3] = {
    Action(4000, 0, 90, mainServo),
    Action(2000, 90, 20, mainServo),
    Action(1000, 20, 180, mainServo),
};

Action secondaryAnimationActions[4] = {
    Action(2000, 0, 20, secondaryServo),
    Action(2000, 20, 180, secondaryServo),
    Action(2600, 80, 90, secondaryServo),
    Action(5200, 90, 0, secondaryServo)
};


Animation<3> mainAnimation = Animation<3>( mainAnimationActions, true, true );
Animation<4> secondaryAnimation = Animation<4>( secondaryAnimationActions );




void setup() {
    Serial.begin(9600);
    Serial.println("16 channel Servo test!");

    pwm.begin();          // Start that bitch up
    pwm.setPWMFreq(333);  // Set frequency for servo drivers

    pinMode(trigPin, OUTPUT); // Set up sonar pins
    pinMode(echoPin, INPUT);

    prevMillis = millis();

    yield();
}

// int randomDeg(int randomNumber) {
//
// }

void changeAnimations() {
    int startDeg, finishDeg, duration;
    bool firstEdit = true;
    for ( int i = 0; i < mainAnimation.getNumActions(); i ++ ) {
        duration = random(1000, 5000);
        startDeg = random(0, 181);
        if (firstEdit) {
            startDeg = random(0, 181);
            finishDeg = random(0, 181);
            firstEdit = false;
        } else {
            startDeg = finishDeg;
            finishDeg = random(0, 181);
        }

        mainAnimation.getAction(i).edit(duration, startDeg, finishDeg);
        Serial.print("Duration: ");
        Serial.print(duration);
        Serial.print(" | start: ");
        Serial.print(startDeg);
        Serial.print(" | finish: ");
        Serial.println(finishDeg);
    }

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
    return distance;
}

void loop() {
    // // Get distance
    // unsigned int distance = getDistance();
    // // If its too big break
    // if (distance > maxDistance ) return;
    // // If its the same as last time, break
    // if (prevDistance == distance) return;
    //
    // prevDistance = distance;
    //
    // // Smooth that bitch out
    // distance = distanceSmoother.smooth(distance);
    //
    // if (distance < 40) {
    //     mainAnimation.check();
    //     // secondaryAnimation.check();
    // }

    mainAnimation.check();

    if (millis() - prevMillis >= 30000) {
        changeAnimations();
        prevMillis = millis();
    }
}
