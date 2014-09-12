#include "Motor.h"

Motor::Motor(int motor_pin){
  attachPin(motor_pin);
}

Motor::Motor(){};

void Motor::attachPin(int motor_pin){
  _motor_pin = motor_pin;
  motor_positive_modifer = 1.0;
  motor_negative_modifer = 2.0;
}

void Motor::reset(){
  _servo.attach(_motor_pin);
}

void Motor::goms(int ms){
  int modifier = 1;
  if (ms>MIDPOINT) modifier = motor_positive_modifer;
  if (ms<MIDPOINT) modifier = motor_negative_modifer;
  int delta = ms-MIDPOINT;
  _servo.writeMicroseconds(constrain(MIDPOINT+delta*modifier,1000,2000));
}

void Motor::stop(){
  _servo.writeMicroseconds(MIDPOINT);
  _servo.detach();
}

bool Motor::attached(){
  return _servo.attached();
}
