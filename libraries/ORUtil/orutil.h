#pragma once

// Includes
#include <Arduino.h>

// Macros
#ifndef NORMALIZE_ANGLE
    #define NORMALIZE_ANGLE_360(a) ((a > 360.0f) ? (a - 360.0f) : ((a < 0.0f) ? (a + 360.0f) : a))
    #define NORMALIZE_ANGLE_180(a) ((a > 180.0f) ? (a - 360.0f) : ((a < -180.0f) ? (a + 360.0f) : a))
#endif

namespace orutil
{
    template<size_t n, uint32_t errorThreshold >
    struct TResultCount
    {
        uint32_t AddResult( uint32_t resultIn )
        {
            ++results[ resultIn ];

            // Any results greater than the error threshold increment the error counter
            if( resultIn > errorThreshold )
            {
                ++errors;
            }

            return resultIn;
        }
        
        uint32_t GetResultCount( uint32_t resultIn )
        {
            return results[ resultIn ];
        }

        uint32_t GetErrorCount()
        {
            return errors;
        }

        void Clear()
        {
            errors = 0;
            memset( results, 0, n * sizeof(uint32_t) );
        }

        void ClearResult( uint32_t resultIn )
        {
            results[ resultIn ] = 0;
        }

    private:
        uint32_t results[ n ] = { 0 };
        uint32_t errors = 0;
    };

    class CTimer
    {
    private:
        // Last time the timer was successfully polled for an elapsed amount of time
        uint32_t m_lastTime_ms;		

    public:
        uint32_t Now();
        bool HasElapsed( uint32_t msIn );
        void Reset();
    };

    // Float<->Int conversion helpers
    constexpr int32_t Encode1K( float valueIn )
    {
        return static_cast<int32_t>( valueIn * 1000.0f );
    }

    constexpr float Decode1K( int32_t valueIn )
    {
        return ( static_cast<float>( valueIn ) * 0.001f );
    }

    // Memory
    int FreeMemory();
}