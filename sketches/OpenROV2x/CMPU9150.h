#pragma once

#include <MPU9150.h>
#include <Arduino.h>
#include "CModule.h"

class CMPU9150 : public CModule
{
public:
    CMPU9150( mpu9150::EAddress addressIn );
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );

private:
    // Device driver
	mpu9150::MPU9150 m_device;

	// Timers
	orutil::CTimer m_statusCheckTimer;
	orutil::CTimer m_telemetryTimer;
    orutil::CTimer m_updateTimer;
    orutil::CTimer m_resetTimer;

    bool m_isInitialized    = false;
    bool m_isDisabled       = false;

    float m_yawOffset 	= 0.0f;
	float m_rollOffset 	= 0.0f;
	float m_pitchOffset = 0.0f;
    
    uint32_t m_maxFailuresPerPeriod = 0;
};