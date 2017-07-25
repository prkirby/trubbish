#ifndef STEPPER_H
#define STEPPER_H

#include "Arduino.h"

class Stepper {
private:
    int startSwitch, endSwitch, stepPin, dirPin;
    unsigned long stepsPerRotation;
    void init( int stepPin, int dirPin, unsigned long stepsPerRotation, int startSwitch, int endSwitch ) {
        this->stepPin = stepPin;
        this->dirPin = dirPin;
        this->stepsPerRotation = stepsPerRotation;
        this->startSwitch = startSwitch;
        this->endSwitch = endSwitch;
    }

public:
    Stepper() {}

    Stepper(int stepPin, int dirPin, unsigned long stepsPerRotation) {
        init(stepPin, dirPin, stepsPerRotation, 0, 0);
    }

    Stepper(int stepPin, int dirPin, unsigned long stepsPerRotation, int startSwitch, int endSwitch ) {
        init(stepPin, dirPin, stepsPerRotation, startSwitch, endSwitch);
    }

    void setUp() {
        // Stepper setup
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
        if (startSwitch) {
            pinMode(startSwitch, INPUT);
        }
        if (endSwitch) {
            pinMode(endSwitch, INPUT);
        }
    }

    float stepsPerDeg() {
        return stepsPerRotation / 360.0;
    }

    unsigned long getStepsPerRotation() {
        return stepsPerRotation;
    }

    void setHigh() {
        digitalWrite(stepPin, HIGH);
    }

    void setLow() {
        digitalWrite(stepPin, LOW);
    }

    void setDir( char dir ) {
        if ( dir == 'f' ) {
            digitalWrite(dirPin, HIGH);
        } else if ( dir == 'b') {
            digitalWrite(dirPin, LOW);
        }
    }

    bool checkStart() {
        if (digitalRead(startSwitch) == LOW) {
            return true;
        } else {
            return false;
        }
    }

    bool checkFinish() {
        if (digitalRead(endSwitch) == LOW) {
            return true;
        } else {
            return false;
        }
    }

};

#endif
