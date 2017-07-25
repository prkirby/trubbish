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
#include "stepper/stepper.hpp"
#include "stepper/stepAction.hpp"

/**
* Timing
*/
unsigned long curMillis, prevMillis;

/**
* Sonar Set Up
*/
int trigPin = 12;
int echoPin = 13;
unsigned int curDistance = 200;
unsigned int maxDistance = 40;
unsigned long prevDistance = 0;
unsigned long prevSonarMillis = 0;
bool userClose = false;

/**
* Smoothing units
*/
Smoother<unsigned int, 3> distanceSmoother;
Smoother<int, 10> calibrationSmoother;

// Servo Drivers
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

/**
* Servo Initialization
* Red: 500 -> 2000; Blue: 800 -> 3500; Analog-> 1000-2000
*/
// Head unit
Servo faceServo = Servo(pwm, 0, 500, 2000);
Servo faceRotateServo = Servo(pwm, 1, 800, 3500);
Servo pedal1Servo = Servo(pwm, 2, 500, 2000);
Servo pedal2Servo = Servo(pwm, 3, 500, 2000);
Servo pedal3Servo = Servo(pwm, 4, 500, 2000);
Servo pedal4Servo = Servo(pwm, 5, 500, 2000);
Servo pedal5Servo = Servo(pwm, 6, 500, 2000);
Servo pedal6Servo = Servo(pwm, 7, 500, 2000);
Servo baseRotateServo = Servo(pwm, 8, 800, 3500);

/**
* Animation Actions
*/
// Face animations
// (runs from 70 to 100)
unsigned int faceAnimationMin = 70;
unsigned int faceAnimationMax = 100;
Action faceAnimationActions[5] = {
    Action(200, faceAnimationMin, faceAnimationMax, faceServo),
    Action(300, faceAnimationMax, faceAnimationMin, faceServo),
    Action(500, faceAnimationMin, faceAnimationMax, faceServo),
    Action(400, faceAnimationMax, faceAnimationMin, faceServo),
    Action(80)
};

// Flower open & close animations
int flowerOpenTime = 400;
int flowerCloseTime = 600;
Action pedal1OpenActions[2] = {
    Action(flowerOpenTime, 0, 140, pedal1Servo),
};
Action pedal2OpenActions[2] = {
    Action(flowerOpenTime, 0, 140, pedal2Servo),
};
Action pedal3OpenActions[2] = {
    Action(flowerOpenTime, 180, 40, pedal3Servo),
};
Action pedal4OpenActions[2] = {
    Action(flowerOpenTime, 0, 140, pedal4Servo),
};
Action pedal5OpenActions[2] = {
    Action(flowerOpenTime, 180, 40, pedal5Servo),
};
Action pedal6OpenActions[2] = {
    Action(flowerOpenTime, 180, 40, pedal6Servo),
};

Action pedal1CloseActions[2] = {
    Action(flowerCloseTime, 140, 0, pedal1Servo),
};
Action pedal2CloseActions[2] = {
    Action(flowerCloseTime, 140, 0, pedal2Servo),
};
Action pedal3CloseActions[2] = {
    Action(flowerCloseTime, 40, 180, pedal3Servo),
};
Action pedal4CloseActions[2] = {
    Action(flowerCloseTime, 140, 0, pedal4Servo),
};
Action pedal5CloseActions[2] = {
    Action(flowerCloseTime, 40, 180, pedal5Servo),
};
Action pedal6CloseActions[2] = {
    Action(flowerCloseTime, 40, 180, pedal6Servo),
};

// Flower wiggle animations
int flowerWiggleTime = 75;
Action flowerWiggleActions[6] = {
    Action(flowerWiggleTime, 140, 120, pedal1Servo),
    Action(flowerWiggleTime, 140, 120, pedal2Servo),
    Action(flowerWiggleTime, 40, 60, pedal3Servo),
    Action(flowerWiggleTime, 140, 120, pedal4Servo),
    Action(flowerWiggleTime, 40, 60, pedal5Servo),
    Action(flowerWiggleTime, 40, 60, pedal6Servo),
};

// Flower Rotate animations
unsigned int faceRotateMin = 0;
unsigned int faceRotateMax = 75;
unsigned int faceRotateHome = 35;
Action faceRotateActions[4] = {
    Action(900, faceRotateHome, faceRotateMax, faceRotateServo),
    Action(1000, faceRotateMax, faceRotateMin, faceRotateServo),
    Action(500, faceRotateMin, faceRotateHome, faceRotateServo),
    Action(500)
};

// Base Rotate Animations
Action baseRotateActions[2] = {
    Action(4000, 60, 130, baseRotateServo),
    Action(4000, 130, 60, baseRotateServo),
};

/**
* Animation Initialization (actions, loop?, reverse?)
*/
Animation<5> faceAnimation = Animation<5>( faceAnimationActions, true, true );
Animation<4> faceRotateAnimation = Animation<4>( faceRotateActions, true, true );
// Pedal Open & Close
Animation<1> pedal1Open = Animation<1>( pedal1OpenActions );
Animation<1> pedal2Open = Animation<1>( pedal2OpenActions );
Animation<1> pedal3Open = Animation<1>( pedal3OpenActions );
Animation<1> pedal4Open = Animation<1>( pedal4OpenActions );
Animation<1> pedal5Open = Animation<1>( pedal5OpenActions );
Animation<1> pedal6Open = Animation<1>( pedal6OpenActions );
Animation<1> pedal1Close = Animation<1>( pedal1CloseActions );
Animation<1> pedal2Close = Animation<1>( pedal2CloseActions );
Animation<1> pedal3Close = Animation<1>( pedal3CloseActions );
Animation<1> pedal4Close = Animation<1>( pedal4CloseActions );
Animation<1> pedal5Close = Animation<1>( pedal5CloseActions );
Animation<1> pedal6Close = Animation<1>( pedal6CloseActions );
// Flower Wiggle Animation
Animation<6> flowerWiggle = Animation<6>( flowerWiggleActions, true, true );
// Base Rotate Animation
Animation<2> baseRotate = Animation<2>( baseRotateActions, false, true);

/**
* Stepper set up
* (stepPin, DirPin, stepsPerRotation)
* (stepPin, DirPin, stepsPerRotation, startSwitch, endSwitch)
*
* (steps, millis, direction, stepper)
* (direction, endSwitch, interval (micros), stepper)
*/
Stepper flowerStepper(3, 4, 200, 8, 9);

StepAction liftFlower('b', 's', 3500, flowerStepper);
StepAction lowerFlower('f', 'f', 3500, flowerStepper);



/**
* Animation Groupings
*/
bool flowerOpen() {
    bool pedal1 = pedal1Open.run();
    bool pedal2 = pedal2Open.run();
    bool pedal3 = pedal3Open.run();
    bool pedal4 = pedal4Open.run();
    bool pedal5 = pedal5Open.run();
    bool pedal6 = pedal6Open.run();
    // After runing them all, see if they are all done
    if (pedal1 && pedal2 && pedal3 && pedal4 && pedal5 && pedal6) {
        return true;
    } else {
        return false;
    }
}

bool flowerClose() {
    bool pedal1 = pedal1Close.run();
    bool pedal2 = pedal2Close.run();
    bool pedal3 = pedal3Close.run();
    bool pedal4 = pedal4Close.run();
    bool pedal5 = pedal5Close.run();
    bool pedal6 = pedal6Close.run();
    // After runing them all, see if they are all done
    if (pedal1 && pedal2 && pedal3 && pedal4 && pedal5 && pedal6) {
        return true;
    } else {
        return false;
    }
}

bool flowerAnimations() {
    bool face = faceAnimation.run();
    bool wiggle = flowerWiggle.run();
    bool rotate = faceRotateAnimation.run();
    if (face && wiggle && rotate) {
        return true;
    } else {
        return false;
    }
}

void flowerAnimationsLoop(bool loop) {
    faceAnimation.setLoop(loop);
    flowerWiggle.setLoop(loop);
    faceRotateAnimation.setLoop(loop);
}

void flowerAnimationsRandomize() {
    faceAnimation.randomize(200, 900, faceAnimationMin, faceAnimationMax, faceRotateMin);
    faceRotateAnimation.randomize(200, 2000, faceRotateMin, faceRotateMax, faceRotateHome);
}

// Resets
void resetFlower() {
    pedal1Open.reset();
    pedal2Open.reset();
    pedal3Open.reset();
    pedal4Open.reset();
    pedal5Open.reset();
    pedal6Open.reset();
    pedal1Close.reset();
    pedal2Close.reset();
    pedal3Close.reset();
    pedal4Close.reset();
    pedal5Close.reset();
    pedal6Close.reset();
}

void resetFlowerAnimations() {
    flowerWiggle.reset();
    faceAnimation.reset();
    faceRotateAnimation.reset();
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

/**
 * Interval Caches
 */
unsigned long flowerWiggleInterval = random(20000, 30000);

/**
* Calibration Utils
*/
int potPin = 0;
int flowerSwitchPin = 7;
int killSwitch = 13;
bool isFlowerOpen = false;
bool calibration = false;


void setup() {

    Serial.begin(9600);

    pwm.begin();          // Start that bitch up
    pwm.setPWMFreq(333);  // Set frequency for servo drivers

    // Set up sonar pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // Calibration tools
    pinMode(flowerSwitchPin, INPUT);
    pinMode(killSwitch, INPUT);

    // Initialize millis counter
    prevMillis = millis();

    // Setup steppers
    flowerStepper.setUp();

    // Setup stepper end stops
    // pinMode(endStopPin, INPUT);

    // Initialize smoothers
    // distanceSmoother.initialize(200);

    yield();

}



void loop() {
    // Check to make sure killswitch not engaged
    if (digitalRead(killSwitch) == HIGH) {


        if (!calibration) {

            // Set the millis for this loop
            curMillis = millis();

            /**
             * Distance grabber
             */
            // Check to see if we are grabbing distance, and set it if we are
            // if ( curMillis - prevSonarMillis > 150 ) {
            //     curDistance = distanceSmoother.smooth(getDistance());
            //     prevSonarMillis = curMillis;
            // }

            // If the user is close and the pedal has already closed
            // if ( userClose && flowerClose() ) {
            //
            //     if ( flowerOpen() ) { // Open the pedal, and once opened..
            //         if ( curDistance >= maxDistance ){ // If far enough away...
            //             userClose = false; // reflect that and reset
            //             resetFlower();
            //         }
            //     }
            // } else if ( !userClose && flowerClose() ) {
            //     if ( curDistance < maxDistance ) {
            //         userClose = true;
            //         // resetFlower();
            //     }
            // }
            //

            if (liftFlower.check() && flowerOpen()) {

                while(millis() - prevMillis < 20000) {
                    flowerAnimations();
                }

                flowerAnimationsLoop(false);


                if ( flowerAnimations() && flowerClose() && lowerFlower.check() ) {
                    resetFlower();
                    liftFlower.reset();
                    lowerFlower.reset();
                    flowerAnimationsRandomize();
                    resetFlowerAnimations();
                    flowerAnimationsLoop(true);
                    prevMillis = millis();
                    // delay(random(20000, 60000));
                }
            }
            //
            // while(flowerWiggle.run()) {}


        } else {
            /**
            * Calibration Testing
            * @param potPin [description]
            */
            // int degree = map(analogRead(potPin), 10, 1014, 0, 180);
            // degree = calibrationSmoother.smooth(degree);
            // Serial.print("Degrees: ");
            // Serial.println(degree);
            // faceServo.servoSet(degree);
            // delay(50);
            //
            // if (isFlowerOpen && digitalRead(flowerSwitchPin) == HIGH) {
            // while (!flowerClose()) {}
            // isFlowerOpen = false;
            // // Serial.println("Closed Flower");
            // resetFlower();
            // } else if (!isFlowerOpen && digitalRead(flowerSwitchPin) == LOW) {
            // while (!flowerOpen()) {}
            // isFlowerOpen = true;
            // Serial.println("Opened Flower");
            // resetFlower();
            // while(!flowerAnimations());
            // resetFlowerAnimations();
            // while(!flowerAnimations());
            // resetFlowerAnimations();
            // }
            // delay(200);

            /**
            * Lift and Lower
            */
            // delay(4000);
            // while(!liftFlower.check()) {}
            // liftFlower.reset();
            // Serial.println("done lifting");
            // while (!flowerOpen()) {}
            //  isFlowerOpen = true;
            //  Serial.println("Opened Flower");
            //  resetFlower();
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while (!flowerClose()) {}
            //  isFlowerOpen = false;
            //  Serial.println("Closed Flower");
            //  resetFlower();
            // while(!baseRotate.run()) {}
            // baseRotate.reset();
            // while(!lowerFlower.check()) {}
            // lowerFlower.reset();
            // Serial.println("done lowering");
            // unsigned long min = 1000L * 60;
            // unsigned long max = 1000L * 60 * 5;
            // delay(random(min , max));

            // /**
            //  * Face Animations Test
            //  */
            //  delay(4000);
            //  while (!flowerOpen()) {}
            //  isFlowerOpen = true;
            //  Serial.println("Opened Flower");
            //  resetFlower();
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while (!flowerClose()) {}
            //  isFlowerOpen = false;
            //  Serial.println("Closed Flower");
            //  resetFlower();


            /**
            * Check Limit Switches
            */
            // Serial.print(flowerStepper.checkStart());
            // Serial.print(" s|f ");
            // Serial.println(flowerStepper.checkFinish());
            // delay(200);

            /**
            * Serv o Pot (3.3V)
            */
            while(!flowerOpen()){}
            while(true) {
            int degree = map(analogRead(potPin), 0, 667, 0, 180);
            faceServo.servoSet(degree);
            Serial.println(degree);
            delay(20);
            }


            // while(!faceRotateAnimation.run()) {}
            // faceRotateAnimation.reset();
            // Serial.println("Ran First");
            // while(!faceRotateAnimation.run()) {}
            // faceRotateAnimation.reset();
            // Serial.println("Ran Second");
            // faceRotateAnimation.randomize(200, 1200, 0, 75, 35);
            // delay(3000);
            // Serial.println("starting next");
        }
    }
}
