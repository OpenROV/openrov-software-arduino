#pragma once

// Includes
#include <Arduino.h>
#include <avr/wdt.h>

namespace NArduinoManager
{
    // Variables
    extern volatile uint8_t	m_wdtResetInfo;

    // Methods
    extern void Initialize();

    extern void EnableWatchdogTimer();
    extern void DisableWatchdogTimer();
    extern void ScanI2C();

    ISR( WDT_vect );
}

