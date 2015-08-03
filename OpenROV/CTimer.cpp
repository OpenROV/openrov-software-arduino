#include "CTimer.h"

CTimer::CTimer()
{
    Reset();
}

// TODO: Now() is not needed, really
uint32_t CTimer::Now()
{
    // Returns the current time in milliseconds
    return millis();
}

boolean CTimer::HasElapsed( uint32_t msIn )
{
    if( ( Now() - m_lastTimeMs ) > msIn )
    {
        // Update the last time to the current time
        m_lastTimeMs = Now();
        return true;
    }

    return false;
}

void CTimer::Reset()
{
    m_startTimeMs	= Now();
    m_lastTimeMs	= m_startTimeMs;
}