#pragma once

// Includes
#include <BNO055.h>
#include "CModule.h"

class CBNO055 : public CModule
{
public:
	CBNO055( I2C *i2cInterfaceIn );
	virtual void Initialize();
	virtual void Update( CCommand &commandIn );

private:
	BNO055 m_bno;

	void InitializeSensor();
};



