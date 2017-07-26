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
int trigPin = 7;
int echoPin = 8;
unsigned int curDistance = 200;
unsigned int maxDistance = 70;
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
    Action(800, faceAnimationMin, faceAnimationMax, faceServo),
    Action(1000, faceAnimationMax, faceAnimationMin, faceServo),
    Action(900, faceAnimationMin, faceAnimationMax, faceServo),
    Action(100, faceAnimationMax, faceAnimationMax - 2, faceServo),
    Action(2200, faceAnimationMax - 2, faceAnimationMin, faceServo)
};
Action faceCloseActions[1] {
    Action(150, faceAnimationMin + 10, faceAnimationMin, faceServo)
};

Action userCloseFaceActions[4] = {
    Action(1500),
    Action(6000, faceAnimationMin, faceAnimationMax, faceServo),
    Action(2000),
    Action(2500, faceAnimationMax, faceAnimationMin, faceServo)
};


// Flower open & close animations
int flowerOpenTime = 800;
int flowerCloseTime = 200;
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
int flowerWiggleTime = 700;
Action flowerWiggleActions[6] = {
    Action(flowerWiggleTime, 140, 120, pedal1Servo),
    Action(flowerWiggleTime, 140, 120, pedal2Servo),
    Action(flowerWiggleTime, 40, 60, pedal3Servo),
    Action(flowerWiggleTime, 140, 120, pedal4Servo),
    Action(flowerWiggleTime, 40, 60, pedal5Servo),
    Action(flowerWiggleTime, 40, 60, pedal6Servo),
};

int closeWiggleTime = 1100;
Action closeWiggleActions[12] = {
    Action(closeWiggleTime, 140, 80, pedal1Servo),
    Action(closeWiggleTime, 140, 80, pedal2Servo),
    Action(closeWiggleTime, 40, 50, pedal3Servo),
    Action(closeWiggleTime, 140, 70, pedal4Servo),
    Action(closeWiggleTime, 40, 60, pedal5Servo),
    Action(closeWiggleTime, 40, 100, pedal6Servo),
    Action(closeWiggleTime, 80, 120, pedal1Servo),
    Action(closeWiggleTime, 80, 130, pedal2Servo),
    Action(closeWiggleTime, 65, 35, pedal3Servo),
    Action(closeWiggleTime, 70, 120, pedal4Servo),
    Action(closeWiggleTime, 60, 20, pedal5Servo),
    Action(closeWiggleTime, 100, 50, pedal6Servo),
};

// Flower Rotate animations
unsigned int faceRotateMin = 0;
unsigned int faceRotateMax = 80;
unsigned int faceRotateHome = 60;
Action faceRotateActions[4] = {
    Action(1500, faceRotateHome, faceRotateMax, faceRotateServo),
    Action(2000, faceRotateMax, faceRotateMin, faceRotateServo),
    Action(1200, faceRotateMin, faceRotateMax, faceRotateServo),
    Action(1500, faceRotateMax, faceRotateHome, faceRotateServo)
};

// Base Rotate Animations
// Action baseRotateActions[2] = {
//     Action(4000, 60, 130, baseRotateServo),
//     Action(4000, 130, 60, baseRotateServo),
// };

/**
* Animation Initialization (actions, loop?, reverse?)
*/
Animation<5> faceAnimation = Animation<5>( faceAnimationActions, true, true );
Animation<1> faceClose = Animation<1>( faceCloseActions, false, false);
Animation<4> faceRotateAnimation = Animation<4>( faceRotateActions, true, true );
// Animation<4> faceCloseAnimation = Animation<4>( fa)
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
Animation<12> closeWiggle = Animation<12>( closeWiggleActions, true, true );
Animation<4> userCloseFace = Animation<4>( userCloseFaceActions, false, false);
// Base Rotate Animation
// Animation<2> baseRotate = Animation<2>( baseRotateActions, false, true);

/**
* Stepper set up
* (stepPin, DirPin, stepsPerRotation)
* (stepPin, DirPin, stepsPerRotation, startSwitch, endSwitch)
*
* (steps, millis, direction, stepper)
* (steps, millis, direction, endSwitch, stepper)
* (direction, endSwitch, interval (micros), stepper)
*/
Stepper flowerStepper(3, 4, 200, 9, 10);

StepAction liftFlower('b', 's', 3500, flowerStepper);
StepAction lowerFlower('f', 'f', 1800, flowerStepper);

Stepper baseStepper(5, 6, 200, 11, 12);

StepAction baseForward(600, 8000, 'f', 'f', baseStepper);
StepAction baseBackward(600, 8000, 'b', 's', baseStepper);



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
    bool face = faceClose.run();
    bool pedal1 = pedal1Close.run();
    bool pedal2 = pedal2Close.run();
    bool pedal3 = pedal3Close.run();
    bool pedal4 = pedal4Close.run();
    bool pedal5 = pedal5Close.run();
    bool pedal6 = pedal6Close.run();

    // After runing them all, see if they are all done
    if (face && pedal1 && pedal2 && pedal3 && pedal4 && pedal5 && pedal6) {
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
    faceAnimation.randomize(900L, 2400L, faceAnimationMin, faceAnimationMax, faceAnimationMin);
    faceRotateAnimation.randomize(1000L, 3000L, faceRotateMin, faceRotateMax, faceRotateHome);
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

    liftFlower.reset();
    lowerFlower.reset();

    flowerWiggle.reset();
    faceAnimation.reset();
    faceRotateAnimation.reset();
    faceClose.reset();
}

void resetFlowerAnimations() {
    flowerWiggle.reset();
    faceAnimation.reset();
    faceRotateAnimation.reset();
}

void closeUserAnimation() {
    faceServo.servoSet(faceAnimationMin);
    faceRotateServo.servoSet(faceRotateHome);
    delay(2000);
    unsigned long prevMillis = millis();
    unsigned long animationTime = random(8000, 16000);
    while (millis() - prevMillis < animationTime && !userCloseFace.run()) {
        closeWiggle.run();
    }
    closeWiggle.reset();
    userCloseFace.reset();
}

/**
 * Flower Lift & Lower
 * @return [description]
 */
bool flowerLiftAnimation() {
    bool isDone = liftFlower.check();
    if( baseForward.check() ) {
        if( baseBackward.check() ) {
            baseForward.reset();
            baseBackward.reset();
        }
    }
    return isDone;
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
unsigned long faceAnimationInterval = random(10000L, 60000L);
unsigned long faceAnimationPrevMillis = millis();
bool faceAnimationPrevSet = false;

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
    baseStepper.setUp();

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


            if ( flowerLiftAnimation() && flowerOpen() ) {

                // If the prevMillis for face animation isnt set, set it.
                if (!faceAnimationPrevSet) {
                    faceAnimationPrevMillis = millis();
                    faceAnimationPrevSet = true;
                    Serial.println("Finished animations");
                }


                    // Run the animation for the duration of the interval
                    if (millis() - faceAnimationPrevMillis < faceAnimationInterval) {
                        flowerAnimations();
                        if (!userClose && getDistance() < maxDistance) {
                            Serial.println("homed in");
                            closeUserAnimation();
                            userClose = true;
                        }
                    } else {
                        userClose = false;
                        Serial.println("finished the interval");
                        // flowerAnimationsLoop(false);
                        // resetFlower();
                        Serial.println("stopped the loop");
                        // Finish animations, close the flower, and lower it
                        // while ( !flowerAnimations() ) {}
                        // Serial.println("Finished Flower Animations");

                        while( !flowerClose() ) {}

                        while( !lowerFlower.check()) {}

                        Serial.println("Finished Flower close");
                        // Clean up and randomize
                        flowerAnimationsRandomize(); // Randomize the flower animations
                        flowerAnimationsLoop(true); // Reset these to loop
                        resetFlower();
                        faceAnimationInterval = random(10000, 30000); // Randomize the face animation interval
                        faceAnimationPrevSet = false; // Make sure this gets set next time round
                        delay(random(10000, 20000)); // Random delay between
                    }
            }

        } else { // Not Calibration End
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

            /**
            * Lift and Lower
            */
        //    while(!flowerClose()) {}
        //     delay(4000);
        //     while(!liftFlower.check()) {}
        //     liftFlower.reset();
        //     Serial.println("done lifting");
        //     delay(4000);
        //     while(!lowerFlower.check()) {}
        //     lowerFlower.reset();
        //     Serial.println("done lowering");


            // /**
            //  * Face Animations Test
            //  */
            //  delay(4000);
            //  while (!flowerOpen()) {}
            //  isFlowerOpen = true;
            //  Serial.println("Opened Flower");
            //  resetFlower();
            //  flowerAnimationsLoop(false);
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while(!flowerAnimations());
            //  resetFlowerAnimations();
            //  while (!flowerClose()) {}
            //  isFlowerOpen = false;
            //  Serial.println("Closed Flower");
            //  resetFlower();

            /**
             * User Close Test
             */

            while (!flowerOpen()) {}
            delay(1000);
            closeUserAnimation();
            delay(5000);

            /**
            * Check Limit Switches
            */
            // Serial.print(flowerStepper.checkStart());
            // Serial.print(" s|f ");
            // Serial.println(flowerStepper.checkFinish());
            // Serial.print(baseStepper.checkStart());
            // Serial.print(" s |(base) f");
            // Serial.println(baseStepper.checkFinish());
            // delay(1000);

            /**
            * Serv o Pot (3.3V)
            */
            // while(!flowerOpen()){}
            // while(true) {
            // int degree = map(analogRead(potPin), 0, 667, 0, 180);
            // faceServo.servoSet(degree);
            // Serial.println(degree);
            // delay(20);
            // }

            /**
            * Base Rotate Test
            */
            // while(!liftFlower.check()) {}
            // delay(2000);
            // Serial.println("Trying to move...");
            // while(!baseForward.check());
            // baseForward.reset();
            // delay(400);
            // while(!baseBackward.check());
            // baseBackward.reset();

            /**
             * Check sonar
             */
            // Serial.println(getDistance());
            // delay(300);

        }
    }
}
