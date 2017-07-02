#ifndef ANIMATION_H
#define ANIMATION_H

#include "action.h"

template <int NUM_ACTIONS >
class Animation {

    private:
        Action actions[NUM_ACTIONS];
        int curIndex;
        bool loop, hasRun, reversed, reverseWhenDone;
        unsigned long prevMillis;

        void init(Action actions[], bool loop, bool reverseWhenDone) {
            for (int i = 0; i < NUM_ACTIONS; i++) {
                this->actions[i] = actions[i];
            }
            this->curIndex = 0;
            this->prevMillis = millis();
            this->loop = loop;
            this->hasRun = false;
            this->reversed = false;
            this->reverseWhenDone = reverseWhenDone;
        }

    public:
        Animation() {
            Action actions[0];
            init(actions, true, false);
        }

        Animation(Action actions[]) {
            init(actions, true, false);
        }

        Animation(Action actions[], bool loop) {
            init(actions, loop, false);
        }

        Animation(Action actions[], bool loop, bool reverseWhenDone) {
            init(actions, loop, reverseWhenDone);
        }

        void reset() {
            this->hasRun = false;
        }

        bool isReversed() {
            return reversed;
        }

        int getNumActions() {
            return NUM_ACTIONS;
        }

        Action& getAction(int index) {
            return actions[index];
        }

        void reverse() {
            Action tmp;

            for ( int i = 0, j = NUM_ACTIONS - 1; i < NUM_ACTIONS / 2; i++, j-- ) {
                tmp = actions[i];
                actions[i] = actions[j];
                actions[j] = tmp;
            }

            for ( int i = 0; i < NUM_ACTIONS; i++ ) {
                actions[i].reverse();
            }

            this->reversed = !this->reversed;
        }

        void check() {
            // If this animation is looping or hasn't run yet
            if ( loop || !hasRun ) {
                if ( millis() - prevMillis >= actions[curIndex].getInterval() ) {
                    prevMillis = millis();
                    // If the action is finished
                    if( actions[curIndex].fire() ) {
                        // Move to next action, and see if it needs to be reset
                        if ( ++curIndex >= NUM_ACTIONS ) {
                            curIndex = 0;
                            hasRun = true;

                            if (reverseWhenDone) {
                                this->reverse();
                            }
                        }
                    }
                }
            }
        }
};
#endif
