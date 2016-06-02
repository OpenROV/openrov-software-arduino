#include "AConfig.h"
#if(HAS_MS5803_XXBA)

#include "CMS5803_XXBA.h"
#include "CMS5803_XX.h"
#include "CTimer.h"
#include "CI2C.h"

#include "NConfigManager.h"
#include "NDataManager.h"

namespace
{
	CMS5803 dev( 512U );

	CTimer DepthSensorSamples;
	CTimer InitTimer;

	float depth_m = 0;
	float depthOffset_m = 0;

	bool isInitialized = false;

	bool GetDepthAndTemperature()
	{
		dev.readSensor();

		return true;
	}

	bool GetCalibrationConstants()
	{
		return dev.GetCalibrationCoefficients();
	}
}


void CMS5803_14BA::Initialize()
{
	NConfigManager::m_capabilityBitmask |= ( 1 << DEPTH_CAPABLE );

	// Reset the chip
	dev.Initialize( false );

	DepthSensorSamples.Reset();
	InitTimer.Reset();
}


void CMS5803_14BA::Update( CCommand& commandIn )
{
	if( isInitialized )
	{
		// Zero the depth value
		if( commandIn.Equals( "dzer" ) )
		{
			dev.m_depthOffset = dev.m_depth;
		}
		else if( commandIn.Equals( "dtwa" ) )
		{
			// Print water type value
			if( NConfigManager::m_waterType == WATERTYPE_FRESH )
			{
				NConfigManager::m_waterType = WATERTYPE_SALT;
				Serial.println( F( "dtwa:1;" ) );
			}
			else
			{
				NConfigManager::m_waterType = WATERTYPE_FRESH;
				Serial.println( F( "dtwa:0;" ) );
			}
		}

		// Read depth value once per second
		if( DepthSensorSamples.HasElapsed( 1000 ) )
		{
			//GetDepthAndTemperature();
			GetDepthAndTemperature();
		}
	}
	else
	{
		// Try to reinitialize every 10 seconds
		if( InitTimer.HasElapsed( 10000 ) )
		{
			Serial.println( "log:Attempting to fetch depth cal coeffs;" );

			if( dev.GetCalibrationCoefficients() == true )
			{
				Serial.println( "log:Depth sensor coeffs obtained!;" );

				isInitialized = true;
			}
		}
	}
}

#endif
