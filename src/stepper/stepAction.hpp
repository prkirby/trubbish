#ifndef STEPACTION_H
#define STEPACTION_H

#include "stepper.hpp"

class StepAction {

private:
    int degrees, curStep = 0, steps;
    unsigned long prevMicros, interval, duration, curMicros;
    char direction;
    bool toStart, toFinish, setHigh = false, hasRun = false, dirSet = false;
    Stepper stepper;
    void init(unsigned long steps, unsigned long duration, char direction) {
        this->duration = duration * 1000;
        this->steps = steps;
        this->interval = this->duration / this->steps / 2;
        this->direction = direction;
        this->prevMicros = micros();
    }

public:
    StepAction(unsigned long steps, unsigned long duration, char direction, Stepper &stepper) {
        this->stepper = stepper;
        init(steps, duration, direction);
    }

    StepAction(unsigned long steps, unsigned long duration, char direction, char endSwitch, Stepper &stepper) {
        this->stepper = stepper;
        if (endSwitch == 'f') {
            this->toFinish = true;
        } else if (endSwitch =='s') {
            this->toStart = true;
        }
        init(steps, duration, direction);
    }

    StepAction(char direction, char endSwitch, unsigned long interval, Stepper &stepper) {
        this->stepper = stepper;
        this->direction = direction;
        // Get the interval from the rpms (rotations per minute)
        this->interval = interval / 2;
        // Serial.println(this->interval);
        if (endSwitch == 'f') {
            this->toFinish = true;
        } else if (endSwitch =='s') {
            this->toStart = true;
        }
        this->direction = direction;
        this->prevMicros = micros();
        // Set this so curStep is always less;
        this->steps = 1;
    }

    // Returns true if animation finished
    bool check() {

        if ( curStep >= steps || hasRun ) {
            return true;
        }
        if (!dirSet) {
            stepper.setDir(direction);
            dirSet = true;
        }

        curMicros = micros();

        if ( curMicros - prevMicros > interval ) {

            if ( ( toStart && stepper.checkStart() ) || ( toFinish && stepper.checkFinish() ) ) {
                hasRun = true;
                return true;
            }

            if ( !setHigh ) {
                stepper.setHigh();
                setHigh = true;
            } else {
                stepper.setLow();
                setHigh = false;
                if ( steps > 1 || ( !toStart && !toFinish ) ) {
                    curStep++;
                }
            }

            prevMicros = micros();
        }

        return false;

    }

    void reset() {
        curStep = 0;
        dirSet = false;
        hasRun = false;
    }


};

#endif
