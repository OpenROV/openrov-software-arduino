#pragma once

#include <Arduino.h>

#define LIBRARY_VERSION	1.1.1

// Controller Direction
#define PID_CONTROLLER_DIRECTION_DIRECT		0
#define PID_CONTROLLER_DIRECTION_INVERSE	1

class CPIDController
{
public:
    // Pointers
    float*		m_pInput;
    float*		m_pOutput;
    float*		m_pSetpoint;

    // Attributes
    uint32_t	m_lastTime_ms;
    uint32_t	m_samplingPeriod_ms;

    int8_t		m_controllerDirection;

    bool		m_isActive;
    bool		m_isInitialized;

    float		m_kp;                  // * (P)roportional Tuning Parameter
    float		m_ki;                  // * (I)ntegral Tuning Parameter
    float		m_kd;                  // * (D)erivative Tuning Parameter

    float		m_displayKp;
    float		m_displayKi;
    float		m_displayKd;

    float		m_pTerm;
    float		m_iTerm;
    float		m_dTerm;

    float		m_lastInput;
    float		m_outputMin;
    float		m_outputMax;

    // Methods
    CPIDController( float kpIn, float kiIn, float kdIn, float outputMinIn, float outputMaxIn, int8_t directionIn, uint32_t samplingPeriodIn_ms );

    void Initialize( float* inputIn, float* outputIn, float* setpointIn );
    virtual void Compute() = 0;
    void Activate();
    void Deactivate();
};

class CPIDControllerAngular : public CPIDController
{
public:
    CPIDControllerAngular( float kpIn, float kiIn, float kdIn, float outputMinIn, float outputMaxIn, int8_t directionIn, uint32_t samplingPeriodIn_ms )
        : CPIDController( kpIn, kiIn, kdIn,	outputMinIn, outputMaxIn, directionIn, samplingPeriodIn_ms )
    {

    }

    virtual void Compute();
};

class CPIDControllerLinear : public CPIDController
{
public:
    CPIDControllerLinear( float kpIn, float kiIn, float kdIn, float outputMinIn, float outputMaxIn, int8_t directionIn, uint32_t samplingPeriodIn_ms )
        : CPIDController( kpIn, kiIn, kdIn,	outputMinIn, outputMaxIn, directionIn, samplingPeriodIn_ms )
    {

    }
    virtual void Compute();
};