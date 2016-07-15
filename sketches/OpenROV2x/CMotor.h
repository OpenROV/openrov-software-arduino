#pragma once

// Includes
#include <Arduino.h>
#include "CServo.h"

class CMotor
{
private:
    CServo	m_servo;
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