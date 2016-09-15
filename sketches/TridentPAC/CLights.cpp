#include "SysConfig.h"
#if(HAS_STD_LIGHTS)

// Includes
#include "CLights.h"
#include "NCommManager.h"
#include "PinDefinitions.h"

// Private helpers
namespace
{
	// Methods
	inline uint8_t ConvertPowerToAnalog( uint32_t powerIn )
	{ 
		// TODO: This is really non-optimal computationally on a non-FPU mcu
		return (uint8_t)( 255.0f * (float)powerIn / 1000000.0f ); 
	}
}

CLights::CLights( uint32_t pwmPinIn, const char *behaviorsIn, const char* traitsIn )
	: CModule( behaviorsIn, traitsIn )
	, m_pwmPin( pwmPinIn, EPinType::ANALOG, EPinDirection::OUTPUT )
{
}

bool CLights::Initialize()
{
	// Make sure the light is off
	light.Write( 0 );

	return true;
}

void CLights::Update()
{
	// Check for messages
	// TODO: NCommManager method that allows you to check for a message specifically for you
	// if( !NCommManager::MessageAvailable( m_uuid ) )
	if( !NCommManager::m_isCommandAvailable )
	{
		// Nothing to do
		return;
	}

	// Handle behavior specific messages
	if( NCommManager::m_currentCommand.IsBehavior( "light" ) )
	{
		// TODO: Come up with some way of handling multiple state requests in one command, i.e.
		// light( uuid, "power:54933", "state:LOW_POWER" )
		if( NCommManager::m_currentCommand.IsCommand( "power" ) )
		{
			// Value should be between 0-1000000, representing 0.0f to 1.0f. Convert to integer 0-255 for analog pin

			// Apply upper and lower bounds
			if( commandIn.m_arguments[2] < 0 )
			{
				m_power = 0;
			}
			if( commandIn.m_arguments[2] > 1000000 )
			{
				m_power = 1000000;
			}			

			light.Write( ConvertPowerToAnalog() );
			
			// Print telemetry
			Serial.print( m_uuid ); 	Serial.print( ":light|power=" );
			Serial.print( m_power ); 	Serial.println( ";" );
		}
	}

	// TODO: Telemtry Timer Loop?
	// State change forced send
}

#endif
