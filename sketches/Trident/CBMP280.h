#pragma once

// Includes
#include <BMP280.h>
#include "CModule.h"

class CI2C;

class CBMP280 : public CModule
{
public:
	CBMP280( CI2C *i2cInterfaceIn );
	virtual void Initialize();
	virtual void Update( CCommand &commandIn );

private:
	BMP280 m_bmp;
	void InitializeSensor();
};



