#pragma once

// Includes
#include <Arduino.h>
#include "SysConfig.h"

namespace NSysManager
{
	// Variables
	extern volatile uint8_t	m_wdtResetInfo;

	// Methods
	extern void Initialize();

	extern void InitializeSerial();
	extern void EnableI2C();
	extern void SetPinModes();
	extern void EnableWatchdogTimer();
	extern void DisableWatchdogTimer();
	extern void ResetWatchdogTimer();
	extern void CheckWatchdogLog();
}

