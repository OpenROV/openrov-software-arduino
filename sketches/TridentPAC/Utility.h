// Contains (with modifications):

// MemoryFree library based on code posted here:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15
//
// Extended by Matthew Murdoch to include walking of the free list.

#pragma once

// Macros
#ifndef NORMALIZE_ANGLE
	#define NORMALIZE_ANGLE(a) ((a > 180.0) ? (a - 360.0) : ((a < -180.0) ? (a + 360.0) : a))
#endif

namespace util
{
	extern "C"
	{
		int FreeMemory();
	}
	
	extern float mapToRange( float valIn, float lowInputRange, float highInputRange, float lowDestRange, float highDestRange );
	extern float mapf( float x, float in_min, float in_max, float out_min, float out_max );
}
