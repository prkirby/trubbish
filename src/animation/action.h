#ifndef ACTION_H
#define ACTION_H

#include "servo/servo.h"

class Action {

private:
    unsigned int duration, interval, startDeg, finishDeg, curDeg;
    bool isForward;
    bool isPause = false;
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

    Action(int duration) {
        this->isPause = true;
        this->interval = duration;
    }

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

    void reverse() {
        unsigned int tmp;
        tmp = startDeg;
        startDeg = finishDeg;
        finishDeg = tmp;
        curDeg = startDeg;
        this->init();
    }

    void edit(unsigned int duration, unsigned int startDeg, unsigned int finishDeg, bool isPause) {
        this->duration = duration;
        this->startDeg = startDeg;
        this->finishDeg = finishDeg;
        this->isPause = isPause;
        this->init();
    }

    /**
     * Moves servo to the next degree
     * @return true if action is finished, otherwise returns false
     */
    bool fire() {
        // If is pause, return true, since the interval is equal to duration
        if (isPause) {
            return true;
        } else { // Otherwise, move the servo in the proper direction, and check if action is finished

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
    }
};

#endif
