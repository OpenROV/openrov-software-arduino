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
    
    MS5837_30BA m_sensor( MS5837_ADC_4096 );

	CTimer m_initTimer;         // 10s timer for initialization attempts
	CTimer m_readTimer;         // 50ms timer for generating data points
	CTimer m_conversionTimer;   // 20ms timer for allowing the sensor to perform conversion before attempting to read the value

	float m_depth_m         = 0;
	float m_depthOffset_m   = 0;
	float m_waterTemp_c     = 0;
	float m_pressure_mbar   = 0;
	
	const int k_maxAttempts = 5;
	int m_initAttempts      = 0;

	ESensorState m_state    = ESensorState::UNINITIALIZED;
	
	void PrintCoefficients()
	{
	    for( int i = 0; i < 8; ++i )
	    {
        	// Print out coefficients
    		Serial.print( "MS5837.C" );
    		Serial.print( i );
    		Serial.print( ":" );
    		Serial.print( m_sensorCoeffs[i] );
    		Serial.println( ";" );
	    }
	    
	    if( m_sensor.m_crcCheckSuccessful )
	    {
	        Serial.println( "MS5837.crcCheck:Success;" );
	    }
	    else
	    {
	        Serial.println( "MS5837.crcCheck:Fail;" );
	    }
	}
}

void CMS5837_30BA::Initialize()
{
	// Attempt to initialize the sensor
	if( m_sensor.Initialize() != 0 )
	{
	    // Announce depth capability
	    NConfigManager::m_capabilityBitmask |= ( 1 << DEPTH_CAPABLE );
	    
	    // Print calibration coefficients and CRC check result
	    PrintCoefficients();
	    
	    // Set the state to IDLE, ready to start fetching data
	    m_state == ESensorState::IDLE;
	}
	
	m_initTimer.Reset();
	
}


void CMS5837_30BA::Update( CCommand& commandIn )
{
    if( m_state == ESensorState::UNINITIALIZED )
    {
        if( m_initAttempts < k_maxAttempts )
        {
            // Make an attempt to initialize the sensor every 10 seconds
            if( m_initTimer.HasElapsed( 10000 ) )
    		{
            	if( m_sensor.Initialize() )
            	{
            	    m_initAttempts = 0;
            	    
            	    // Announce depth capability
            	    NConfigManager::m_capabilityBitmask |= ( 1 << DEPTH_CAPABLE );
            	    
            	    // Print calibration coefficients and CRC check result
            	    PrintCoefficients();
            	    
            	    // Set the state to IDLE, ready to start fetching data
            	    m_state == ESensorState::IDLE;
            	}
            	else
            	{
            	    m_initTimer.Reset();
            	    m_initAttempts++;
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
            	m_depthOffset = m_depth_m;
            }
            else if( commandIn.Equals( "dtwa" ) )
            {
                // TODO: Make this a command to change water type properly
                
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
                m_sensor.StartConversion( MS5837_MEAS_PRESSURE );
                
                // Reset the read timer
                m_readTimer.Reset();
                
                // Start the conversion timer
                m_conversionTimer.Reset();
                
                m_state = ESensorState::CONVERTING_PRESSURE;
    		}
        }
        else if( m_state == ESensorState::CONVERTING_PRESSURE )
        {
            // Wait 20ms for a conversion to complete
            if( m_conversionTimer.HasElapsed( 10 ) )
    		{
    		    // Read the pressure value
                m_sensor.Read( MS5837_MEAS_PRESSURE );
                
                // Kick off the temperature conversion
                m_sensor.StartConversion( MS5837_MEAS_TEMPERATURE );
                
                m_conversionTimer.Reset();
                
                m_state = ESensorState::CONVERTING_TEMPERATURE;
    		}
        }
        else if( m_state == ESensorState::CONVERTING_TEMPERATURE )
        {
            // Wait 20ms for a conversion to complete
            if( m_conversionTimer.HasElapsed( 10 ) )
    		{
    		    // Read the temperature value
                m_sensor.Read( MS5837_MEAS_TEMPERATURE );
                
                // Perform calculations
                m_sensor.CalculateOutputs();
                
                // Get and report results
                m_depth_m       = m_sensor.m_depth_m - m_depthOffset_m;
                m_waterTemp_c   = m_sensor.m_temperature_c;
                m_pressure_mbar = m_sensor.m_pressure_mbar;
                
                Serial.print( "MS5837.depth_m:" );
    		    Serial.print( m_depth_m );
    		    Serial.println( ";" );
    		    
    		    Serial.print( "MS5837.waterTemp_c:" );
    		    Serial.print( m_waterTemp_c );
    		    Serial.println( ";" );
    		    
    		    Serial.print( "MS5837.pressure_mbar:" );
    		    Serial.print( m_pressure_mbar );
    		    Serial.println( ";" );
    		    
    		    NDataManager::m_environmentData.TEMP = m_waterTemp_c;
    		    NDataManager::m_environmentData.PRES = m_pressure_mbar;
    		    NDataManager::m_environmentData.DEEP = m_depth_m;
                
                m_state = ESensorState::IDLE;
    		}
        }
    }
}

#endif