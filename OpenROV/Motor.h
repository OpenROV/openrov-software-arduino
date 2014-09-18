#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "openrov_servo.h"
#include <Arduino.h>
#define MIDPOINT 1500

class Motor {
  private:
    Servo _servo;
    int _motor_pin;

  public:
    Motor(int motor_pin);
    Motor();
    void attachPin(int motor_pin);
    int goms(int ms);
    void reset();
    void stop();
    bool attached();
    float motor_positive_modifer;
    float motor_negative_modifer;
    int motor_deadzone_negative;
    int motor_deadzone_positive;
};

#endif
