#ifndef ANIMATION_H
#define ANIMATION_H

#include "action.h"

//TODO: Double check reset, as it doesn't seem to be doing its job.

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
            init(actions, false, false);
        }

        Animation(Action actions[]) {
            init(actions, false, false);
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

        void setLoop( bool isLooping ) {
            this->loop = isLooping;
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

        /**
         * Randomize the actions for the animation
         */
        void randomize(unsigned int minDur, unsigned int maxDur, unsigned int minDeg, unsigned int maxDeg, unsigned int endDeg = false) {
            unsigned int startDeg, finishDeg, duration;
            bool firstEdit = true;
            bool isPause = false;
            for ( int i = 0; i < this->getNumActions(); i ++ ) {

                duration = random(minDur, maxDur);

                if ( !firstEdit && i != this->getNumActions() - 1 && random(1, 6) == 1 ) { // If its not first or last, give it a one in 5 chance of being a pause
                    isPause = true;
                    this->getAction(i).edit(duration, minDeg, maxDeg, isPause);
                    Serial.println("Is Pause");
                }

                else { // Otherwise, give it proper degrees to work with

                    if (firstEdit) {
                        if (endDeg) {
                            startDeg = endDeg;
                        } else {
                            startDeg = random(minDeg, maxDeg);
                        }
                        firstEdit = false;
                    } else {
                        startDeg = finishDeg;
                    }

                    if (i == this->getNumActions() - 1 && endDeg) {
                        if (startDeg == endDeg) {
                            endDeg++; // Double check they arent the same, or else we will get stuck in a loop.
                        }
                        finishDeg = endDeg;

                    } else {
                        finishDeg = random(minDeg, maxDeg);
                        if (finishDeg == startDeg) { // Same check here
                            finishDeg++;
                        }
                    }

                    this->getAction(i).edit(duration, startDeg, finishDeg, isPause);
                }
            }
        }

        bool run() {
            // If this animation is looping or hasn't run yet
            if ( loop || !hasRun ) {
                if ( millis() - prevMillis >= actions[curIndex].getInterval() ) {
                    prevMillis = millis();
                    // If the action is finished
                    if( actions[curIndex].fire() ) {
                        // Move to next action, and see if it needs to be reset
                        if ( ++curIndex >= NUM_ACTIONS ) {
                            curIndex = 0;

                            if (!loop) { // Make sure animations run to completion after loop shutting off.
                                hasRun = true;
                            }

                            if (reverseWhenDone) {
                                this->reverse();
                            }
                        }
                    }
                }
            } else {

                return true;

            }

            return false;
        }


};
#endif
