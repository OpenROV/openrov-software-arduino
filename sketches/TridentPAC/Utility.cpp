#include "SysConfig.h"

// -----------------------------------------
// Setup code for FreeMemory() function
// -----------------------------------------


// -----------------------------------------
// Utility functions contained here
// -----------------------------------------

#include "Utility.h"

namespace util
{
	int FreeMemory()
	{
		return 0;
	}

	float mapToRange( float valIn, float lowInputRange, float highInputRange, float lowDestRange, float highDestRange )
	{
		return ( lowDestRange ) + ( valIn - lowInputRange ) * ( highDestRange - lowDestRange ) / ( highInputRange - lowInputRange );
	}
	
	float mapf( float x, float in_min, float in_max, float out_min, float out_max )
	{
		return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
	}
}
