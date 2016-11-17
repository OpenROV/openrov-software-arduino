#pragma once

// Includes
#include <BNO055.h>
#include <orutil.h>
#include "CModule.h"

class CBNO055 : public CModule
{
public:
	CBNO055( I2C *i2cInterfaceIn, bno055::EAddress addressIn );
	void Initialize();
	void Update( CCommand &commandIn );

private:
	// Device driver
	bno055::BNO055 m_device;

	// Timers
	orutil::CTimer m_statusCheckTimer;
	orutil::CTimer m_telemetryTimer;
	orutil::CTimer m_calibTimer;

	float m_yawOffset 	= 0.0f;
	float m_rollOffset 	= 0.0f;
	float m_pitchOffset = 0.0f;

	uint32_t m_maxFailuresPerPeriod = 0;
};



