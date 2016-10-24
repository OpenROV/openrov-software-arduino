#pragma once

#include "CCommand.h"
#include <Arduino.h>

class CModule
{
public:
	// Methods
	CModule();

	// Pure virtuals - These methods must be instantiated in your derived class!
	virtual void Initialize() = 0;
	virtual void Update( CCommand& commandIn ) = 0;
};