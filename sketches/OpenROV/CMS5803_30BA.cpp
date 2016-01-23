#include "SysConfig.h"
#if(HAS_MS5803_30BA)

#include "CMS5803_30BA.h"
#include <MS5803_30.h>

#include "CTimer.h"

#include "NVehicleManager.h"
#include "NDataManager.h"

namespace
{
	// Variables
	MS5803_30 dev( 512U );

	CTimer DepthSensorSamples;
	CTimer InitTimer;

	float m_depth 			= 0;
	float m_depthOffset 	= 0;

	bool isInitialized 		= false;

	// Methods
	float CalculateDepth()
	{
		float mbar = dev.readSensor();
		
		//FreshWater
		m_depth = ( mbar - 1013.25f ) * 1.019716f / 100.0f;
	
		// Calculate the depth using pressure
		if( NVehicleManager::m_waterType == WATERTYPE_FRESH )
		{
			//FreshWater
			m_depth = ( mbar - 1013.25f ) * 1.019716f / 100.0f;
		}
		else
		{
			// Salt Water
			// See Seabird App Note #69 for details
			// 9.72659 / 9.780318 = 0.9945
			m_depth = ( mbar - 1013.25f ) * 0.9945f / 100.0f;
		}
	
		return ( m_depth - m_depthOffset );
	}
}


void CMS5803_30BA::Initialize()
{
	// Reset the chip
	dev.Initialize();

	// Reset timers
	DepthSensorSamples.Reset();
	InitTimer.Reset();
}


void CMS5803_30BA::Update( CCommand& commandIn )
{
	if( isInitialized )
	{
		// Zero the depth value
		if( commandIn.Equals( "dzer" ) )
		{
			m_depthOffset = m_depth;
		}
		else if( commandIn.Equals( "dtwa" ) )
		{
			// Print water type value
			if( NVehicleManager::m_waterType == WATERTYPE_FRESH )
			{
				NVehicleManager::m_waterType = WATERTYPE_SALT;
				Serial.println( F( "dtwa:1;" ) );
			}
			else
			{
				NVehicleManager::m_waterType = WATERTYPE_FRESH;
				Serial.println( F( "dtwa:0;" ) );
			}
		}

		// Read depth value once per second
		if( DepthSensorSamples.HasElapsed( 100 ) )
		{
			//GetDepthAndTemperature();
			NDataManager::m_navData.DEEP = CalculateDepth();
		}
	}
	else
	{
		// Try to reinitialize every 10 seconds
		if( InitTimer.HasElapsed( 5000 ) )
		{
			Serial.println( "log:Initializing depth sensor;" );
			
			isInitialized = dev.Initialize();
			
			if( isInitialized )
			{
				Serial.println( "log:Success!;" );
			}
			else
			{
				Serial.println( "log:Failed!!;" );
			}
		}
	}
}

#endif
