#include "orutil.h"

#if defined( ARDUINO_ARCH_AVR )
    extern unsigned int __heap_start;
    extern void*		__brkval;

    // The free list structure as maintained by the avr-libc memory allocation routines.
    struct __freelist
    {
        size_t sz;
        struct __freelist* nx;
    };

    // The head of the free list structure
    extern struct __freelist* __flp;

    // Calculates the size of the free list
    int freeListSize()
    {
        struct __freelist* current;
        int total = 0;

        for( current = __flp; current; current = current->nx )
        {
            // Add two bytes for the memory block's header
            total += 2;
            total += ( int ) current->sz;
        }

        return total;
    }
#endif

namespace orutil
{
    // CTimer
    uint32_t CTimer::Now()
    {
        // Returns the current time in milliseconds
        return millis();
    }

    bool CTimer::HasElapsed( uint32_t msIn )
    {
        if( ( Now() - m_lastTime_ms ) > msIn )
        {
            // Update the last time to the current time
            m_lastTime_ms = Now();
            return true;
        }

        return false;
    }

    void CTimer::Reset()
    {
        m_lastTime_ms = Now();
    }

    #if defined( ARDUINO_ARCH_AVR )
        int FreeMemory()
        {
            int free_memory;

            if( ( int )__brkval == 0 )
            {
                free_memory = ( ( int )&free_memory ) - ( ( int )&__heap_start );
            }
            else
            {
                free_memory = ( ( int )&free_memory ) - ( ( int )__brkval );
                free_memory += freeListSize();
            }

            return free_memory;
        }

    #endif

}