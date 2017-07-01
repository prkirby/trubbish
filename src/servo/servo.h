/**
 * The wrapper class for Adafruit_PWMServoDriver servos
 * Allows for callibration on specific servos.
 */
 #ifndef SERVO_H
 #define SERVO_H

 #include "Wire.h"
 #include "Adafruit_PWMServoDriver.h"

 class Servo {
   private:
     Adafruit_PWMServoDriver driver;
     int address, pwmMin, pwmMax;
     int prevDeg = 0;

  public:
    Servo() {
        this->address = 1;
        this->pwmMin = 200;
        this->pwmMax = 2400;
    }
    Servo(Adafruit_PWMServoDriver &driver, int address, int pwmMin, int pwmMax) {
      this->driver = driver;
      this->address = address;
      this->pwmMin = pwmMin;
      this->pwmMax = pwmMax;
    }

    void servoSet(int deg) {
        int pulseLength = map(deg, 0, 180, pwmMin, pwmMax);
        driver.setPWM(address, 0, pulseLength);
    }

    Servo& move(int deg, int movementTime) {

      unsigned long ms = movementTime / abs(deg - prevDeg);
      unsigned long prevMillis = millis();
      unsigned long curMillis;

      if ( prevDeg < deg ) {
          for ( int i = prevDeg + 1; i <= deg; i++ ) {
             while(true) {
                 curMillis = millis();
                 if (curMillis - prevMillis == ms) {
                     servoSet(i);
                     break;
                 } else {

                 }
             }

          }
      } else if ( prevDeg > deg ) {
          for ( int i = prevDeg - 1; i >= deg; i-- ) {
              servoSet(i);
          }
      }

      prevDeg = deg;

      return *this;
    }
 };

 #endif
