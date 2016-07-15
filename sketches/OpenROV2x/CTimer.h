#pragma once

// Includes
#include <Arduino.h>

class CTimer
{
private:
    uint32_t m_startTimeMs;		// Time the timer came into existence
    uint32_t m_lastTimeMs;		// Last time the timer was successfully polled for an elapsed amount of time

public:
    CTimer();

    uint32_t Now();
    boolean HasElapsed( uint32_t msIn );
    void Reset();
};