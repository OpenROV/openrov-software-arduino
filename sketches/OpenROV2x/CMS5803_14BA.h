#pragma once

// Includes
#include <MS5803_14BA.h>
#include "CModule.h"

class CMS5803_14BA : public CModule
{
public:
	CMS5803_14BA( I2C *i2cInterfaceIn );
	void Initialize();
	void Update( CCommand& commandIn );

private:
	MS5803_14BA m_sensor;

	void PrintCoefficients();
};