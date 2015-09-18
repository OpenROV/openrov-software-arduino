#pragma once

// Includes
#include "CModule.h"

class CAltServo : public CModule
{
public:
	void Initialize();
	void Update( CCommand& commandIn );
};
