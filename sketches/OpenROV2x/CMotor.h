#pragma once

// Includes
#include <Arduino.h>
#include <Servo.h>

// Motors
#define MOTOR_TARGET_MIN_US						1000
#define MOTOR_TARGET_NEUTRAL_US					1500
#define MOTOR_TARGET_MAX_US						2000
#define MOTOR_DEFAULT_POSITIVE_MOD				1.0f
#define MOTOR_DEFAULT_NEGATIVE_MOD				1.0f
#define MOTOR_DEFAULT_POSITIVE_DEADZONE_BUFFER	50
#define MOTOR_DEFAULT_NEGATIVE_DEADZONE_BUFFER	50

class CMotor
{
private:
    Servo	m_servo;
    int		m_motorPin;

public:
    float	m_positiveModifier;
    float	m_negativeModifier;
    int		m_positiveDeadzoneBuffer;
    int		m_negativeDeadzoneBuffer;


    CMotor( int motorPinIn );
    CMotor();

    void SetPin( int motorPinIn );
    void Activate();
    void Deactivate();

    int SetMotorTarget( int ms );
    bool IsActive();
};