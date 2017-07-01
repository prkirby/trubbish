#ifndef ACTION_H
#define ACTION_H

#include "servo/servo.h"

class Action {

private:
    unsigned int duration, interval, startDeg, finishDeg, curDeg;
    bool isForward;
    Servo servo;
    void init() {
        int difference = finishDeg - startDeg;
        if (difference < 0) {
            difference *= -1;
            isForward = false;
        } else {
            isForward = true;
        }
        interval = duration / difference;
    }

public:
    Action() {}
    Action(int duration, int startDeg, int finishDeg, Servo &servo) {
        this->duration = duration;
        this->startDeg = startDeg;
        this->finishDeg = finishDeg;
        this->curDeg = startDeg;
        this->servo = servo;
        this->init();
    }

    unsigned int getInterval() {
        return this->interval;
    }

    /**
     * Moves servo to the next degree
     * @return true if action is finished, otherwise returns false
     */
    bool fire() {
        if (isForward) {
            curDeg++;
            this->servo.servoSet(curDeg);
        } else {
            curDeg--;
            this->servo.servoSet(curDeg);
        }
        if ( (isForward && curDeg >= finishDeg ) || (!isForward && curDeg <= finishDeg ) ) {
            curDeg = startDeg;
            return true;
        } else {
            return false;
        }
    }
};

#endif
