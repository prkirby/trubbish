/**
 * The wrapper class for Adafruit_PWMServoDriver servos
 * Allows for callibration on specific servos.
 */

 #include "Wire.h"
 #include "Adafruit_PWMServoDriver.h"

 class Servo {
   private:
     Adafruit_PWMServoDriver driver;
     int address, pwmMin, pwmMax;

  public:
    Servo(Adafruit_PWMServoDriver &driver, int address, int pwmMin, int pwmMax) {
      this->driver = driver;
      this->address = address;
      this->pwmMin = pwmMin;
      this->pwmMax = pwmMax;
    }

    void move(int percentage) {
      int pulseLength = map(percentage, 0, 100, pwmMin, pwmMax);
      driver.setPWM(address, 0, pulseLength);
    }
 };
