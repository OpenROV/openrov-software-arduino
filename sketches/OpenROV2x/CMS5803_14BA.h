#pragma once

// Includes
#include <MS5803_14BA.h>
#include <orutil.h>
#include "CModule.h"

class CMS5803_14BA : public CModule
{
public:
	CMS5803_14BA( I2C *i2cInterfaceIn, ms5803_14ba::EAddress addressIn );
	void Initialize();
	void Update( CCommand& commandIn );

private:
	// Device driver
	ms5803_14ba::MS5803_14BA m_device;

	// Timers
	orutil::CTimer m_statusCheckTimer;
	orutil::CTimer m_telemetryTimer;

	float m_depthOffset_m = 0.0f;
	uint32_t m_maxFailuresPerPeriod = 0;
	bool m_hasLock = false;
};