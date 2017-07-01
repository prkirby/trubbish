#ifndef ANIMATION_H
#define ANIMATION_H

#include "action.h"

template <int NUM_ACTIONS >
class Animation {

    private:
        Action actions[NUM_ACTIONS];
        int curIndex;
        unsigned long prevMillis;

    public:
        Animation() {
            
        }

        Animation(Action actions[]) {
            for (int i = 0; i < NUM_ACTIONS; i++) {
                this->actions[i] = actions[i];
            }
            this->curIndex = 0;
            this->prevMillis = millis();
        }

        void check() {
            if ( millis() - prevMillis >= actions[curIndex].getInterval() ) {
                prevMillis = millis();
                // If the action is finished
                if( actions[curIndex].fire() ) {
                    // Serial.println("move to next action");
                    if ( ++curIndex >= NUM_ACTIONS ) {
                        curIndex = 0;
                    }
                } else {
                    //carry on
                }
            }
        }
};
#endif
