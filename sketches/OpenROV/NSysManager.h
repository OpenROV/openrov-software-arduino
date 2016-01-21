#pragma once

// Includes
#include <Arduino.h>
#include "Config.h"

// Includes
#if MCUARCH == MCUARCH_AVR
	#include <avr/wdt.h>
#endif

namespace NSysManager
{
	// Variables
	extern volatile uint8_t	m_wdtResetInfo;

	// Methods
	extern void Initialize();

	extern void EnableWatchdogTimer();
	extern void DisableWatchdogTimer();
	extern void ResetWatchdogTimer();
	extern void CheckWatchdogLog();

#if MCUARCH == MCUARCH_AVR
	ISR( WDT_vect );
#endif

}

