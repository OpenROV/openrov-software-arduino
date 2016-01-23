#pragma once

// Includes
#include "CModule.h"

#define BNO_WIRE_INTERFACE_0	0
#define BNO_WIRE_INTERFACE_1	1
#define BNO_WIRE_INTERFACE_2	2
#define BNO_WIRE_INTERFACE_3	3
#define BNO_WIRE_INTERFACE_4	4
#define BNO_WIRE_INTERFACE_5	5

class CBNO055 : public CModule
{
public:
	virtual void Initialize();
	virtual void Update( CCommand &commandIn );
};



