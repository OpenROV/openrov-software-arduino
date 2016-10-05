// Contains (with modifications):

// MemoryFree library based on code posted here:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15
//
// Extended by Matthew Murdoch to include walking of the free list.

#pragma once

#include <stdint.h>

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

	// Float<->Int conversion helpers
    constexpr int32_t Encode1K( float valueIn )
    {
        return static_cast<int32_t>( valueIn * 1000.0f );
    }

    constexpr float Decode1K( int32_t valueIn )
    {
        return ( static_cast<float>( valueIn ) * 0.001f );
    }
}
