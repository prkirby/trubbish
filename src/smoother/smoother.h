/**
* Class to smooth out input streams of integers
*/

template < class T, int ARRAY_LEN >
class Smoother {

private:
    int readIndex = 0;
    int bufferSize = ARRAY_LEN;
    T total = 0;
    T average = 0;
    T readings[ARRAY_LEN];
    void init() {
        // Initialize all readings to 0
        for (int i = 0; i < bufferSize; i++) {
            readings[i] = 0;
        }
    }

public:
    // Default buffer size is 10;
    Smoother() {
        init();
    }

    T smooth(T value) {
        total = total - readings[readIndex];
        // read from the sensor:
        readings[readIndex] = value;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex++;

        // if we're at the end of the array...
        if (readIndex >= bufferSize) {
            // ...wrap around to the beginning:
            readIndex = 0;
        }

        // calculate the average:
        average = total / bufferSize;
        return average;
    }

    void initialize(T value) {
        for (int i = 0; i < ARRAY_LEN; i++) {
            readings[i] = value;
        }
    }
};
