#include "AConfig.h"
#if(HAS_MS5837_30BA)

#include "CMS5837_30BA.h"
#include "LibMS5837_30BA.h"
#include "CTimer.h"
#include "CI2C.h"

#include "NCommManager.h"
#include "NConfigManager.h"
#include "NDataManager.h"

namespace
{
    enum class ESensorState
    {
        UNINITIALIZED,
        IDLE,
        CONVERTING_PRESSURE,
        CONVERTING_TEMPERATURE
    };
    
    MS5837_30BA m_sensor( MS5837_RES_4096 );

	CTimer m_initTimer;         // 10s timer for initialization attempts
	CTimer m_readTimer;         // 50ms timer for generating data points
	CTimer m_conversionTimer;   // 20ms timer for allowing the sensor to perform conversion before attempting to read the value

	float m_depth_m         = 0;
	float m_depthOffset_m   = 0;
	
	const int k_maxRetries  = 3;
	int m_retries           = 0;

	ESensorState m_state    = ESensorState::UNINITIALIZED;
}

void CMS5837_30BA::Initialize()
{
    // Announce depth capability
	NConfigManager::m_capabilityBitmask |= ( 1 << DEPTH_CAPABLE );
	
	// Attempt to initialize the sensor
	if( m_sensor.Initialize() )
	{
	    // Set the state to IDLE, ready to start fetching data
	    m_state == ESensorState::IDLE;
	}
	
	m_initTimer.Reset();
}


void CMS5837_30BA::Update( CCommand& commandIn )
{
    // Handle commands
    
    if( m_state == ESensorState::UNINITIALIZED )
    {
        if( m_retries < k_maxRetries )
        {
            // Make an attempt to initialize the sensor every 10 seconds
            if( m_initTimer.HasElapsed( 10000 ) )
    		{
            	if( m_sensor.Initialize() )
            	{
            	    // Set the state to IDLE, ready to start fetching data
            	    m_state == ESensorState::IDLE;
            	}
            	else
            	{
            	    m_initTimer.Reset();
            	    m_retries++;
            	}
    		}
        }
    }
    else
    {
        // Handle commands
        if( NCommManager::m_isCommandAvailable )
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
        }
        
        // TODO: Handle error checking for sensor actions, since they can fail due to I2C problems
        // Handle sensor update process
        if( m_state == ESensorState::IDLE )
        {
            // Kick off a conversion every 50ms
            if( m_readTimer.HasElapsed( 50 ) )
    		{
                m_sensor.StartConversion();
                
                // Reset the read timer
                m_readTimer.Reset();
                
                // Start the conversion timer
                m_conversionTimer.Reset();
                
                m_state = ESensorState::CONVERTING;
    		}
        }
        else if( m_state == ESensorState::CONVERTING )
        {
            // Wait 20ms for a conversion to complete
            if( m_conversionTimer.HasElapsed( 20 ) )
    		{
                m_sensor.Read();
                
                // Perform calculations
                
                // Report results
                
                m_state = ESensorState::IDLE;
    		}
        }
    }
}

#endif