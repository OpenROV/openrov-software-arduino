#include "SysConfig.h"
#if(HAS_STD_LIGHTS)

// Includes
#include <Arduino.h>
#include "CLights.h"
#include "CPin.h"
#include "NCommManager.h"
#include "NVehicleManager.h"
#include "PinDefinitions.h"

namespace
{
	CPin light( "light", PIN_STANDARD_LIGHTS, CPin::kAnalog, CPin::kOutput );
}

void CLights::Initialize()
{
	NVehicleManager::m_capabilityBitmask |= ( 1 << LIGHTS_CAPABLE );

	light.Reset();
	light.Write( 0 );
}

void CLights::Update( CCommand& commandIn )
{
	// Check for messages
	if( !NCommManager::m_isCommandAvailable )
	{
		return;
	}

	// Handle messages
	if( commandIn.Equals( "ligt" ) )
	{
		// Should be between 0-255, with 255 being full brightness
		int value = commandIn.m_arguments[1];

		// Bounds corrections
		if( value < 0 )
		{
			value = 0;
		}
		if( value > 255 )
		{
			value = 255;
		}
		
		light.Write( value );
		
		Serial.print( F( "LIGT:" ) );
		Serial.print( value );
		Serial.print( ';' );
	}
}

#endif
