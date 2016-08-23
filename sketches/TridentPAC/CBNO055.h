#pragma once

// Includes
#include <BNO055.h>
#include "CModule.h"

class CI2C;

class CBNO055 : public CModule
{
public:
	CBNO055( CI2C *i2cInterfaceIn );
	virtual void Initialize();
	virtual void Update( CCommand &commandIn );

private:
	BNO055 m_bno;

	void InitializeSensor();
};



