#include "SysConfig.h"

// -----------------------------------------
// Setup code for FreeMemory() function
// -----------------------------------------

#if MCUARCH == MCUARCH_AVR

extern unsigned int __heap_start;
extern void		*__brkval;

// The free list structure as maintained by the avr-libc memory allocation routines.
struct __freelist
{
	size_t sz;
	struct __freelist *nx;
};

// The head of the free list structure
extern struct __freelist *__flp;

// Calculates the size of the free list
int freeListSize()
{
	struct __freelist *current;
	int total = 0;

	for( current = __flp; current; current = current->nx )
	{
		// Add two bytes for the memory block's header
		total += 2;
		total += ( int ) current->sz;
	}

	return total;
}

#elif MCUARCH == MCUARCH_SAMD
// Nothing yet
#endif


// -----------------------------------------
// Utility functions contained here
// -----------------------------------------

#include "Utility.h"

namespace util
{
	int FreeMemory()
	{
		// AVR
#if MCUARCH == MCUARCH_AVR

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

		// SAMD
#elif MCUARCH == MCUARCH_SAMD
		return 0;
#endif
	}
	
	float mapf( long x, long in_min, long in_max, long out_min, long out_max )
	{
		return ( float )( x - in_min ) * ( out_max - out_min ) / ( float )( in_max - in_min ) + out_min;
	}

	float mapff( float x, float in_min, float in_max, float out_min, float out_max )
	{
		return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
	}
}
