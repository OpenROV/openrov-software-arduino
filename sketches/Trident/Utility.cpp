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
	
	float mapf( long x, long in_min, long in_max, long out_min, long out_max )
	{
		return ( float )( x - in_min ) * ( out_max - out_min ) / ( float )( in_max - in_min ) + out_min;
	}
}
