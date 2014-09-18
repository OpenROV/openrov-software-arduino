#include "Motor.h"

Motor::Motor(int motor_pin){
  attachPin(motor_pin);
}

Motor::Motor(){};

void Motor::attachPin(int motor_pin){
  _motor_pin = motor_pin;
  motor_positive_modifer = 1.0;
  motor_negative_modifer = 2.0;
  motor_deadzone_negative = 50;
  motor_deadzone_positive = 50;
}

void Motor::reset(){
  _servo.attach(_motor_pin);
}

int Motor::goms(int ms){
  int modifier = 1;
  if (ms>MIDPOINT) modifier = motor_positive_modifer;
  if (ms<MIDPOINT) modifier = motor_negative_modifer;
  int delta = ms-MIDPOINT;
  //map around deadzone
  int predeadzonems = constrain(MIDPOINT+delta*modifier,1000,2000);
  int finalms = MIDPOINT;
  if (predeadzonems < MIDPOINT){
     finalms = map(predeadzonems, 1000, MIDPOINT, 1000, MIDPOINT-motor_deadzone_negative);
  } else if (predeadzonems > MIDPOINT) {
     finalms = map(predeadzonems, MIDPOINT, 2000, MIDPOINT+motor_deadzone_positive, 2000);
  }

  _servo.writeMicroseconds(constrain(finalms,1000,2000));
  return _servo.readMicroseconds();
}

void Motor::stop(){
  _servo.writeMicroseconds(MIDPOINT);
  _servo.detach();
}

bool Motor::attached(){
  return _servo.attached();
}
