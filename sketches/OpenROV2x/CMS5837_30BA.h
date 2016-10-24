#pragma once

// Includes
#include <MS5837_30BA.h>
#include <orutil.h>
#include "CModule.h"

class CMS5837_30BA : public CModule
{
public:
	CMS5837_30BA( I2C *i2cInterfaceIn, ms5837_30ba::EAddress addressIn );
	void Initialize();
	void Update( CCommand& commandIn );

private:
	// Device driver
	ms5837_30ba::MS5837_30BA m_device;

	// Timers
	orutil::CTimer m_statusCheckTimer;
	orutil::CTimer m_telemetryTimer;

	float m_depthOffset_m = 0.0f;
	uint32_t m_maxFailuresPerPeriod = 0;
	bool m_hasLock = false;
};