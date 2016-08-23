#pragma once

// Includes
#include <MS5837_30BA.h>
#include "CModule.h"

class CI2C;

class CMS5837_30BA : public CModule
{
public:
	CMS5837_30BA( CI2C *i2cInterfaceIn );
	void Initialize();
	void Update( CCommand& commandIn );

private:
	MS5837_30BA m_sensor;

	void PrintCoefficients();
};