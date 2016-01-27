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
	CPin elight( "elight", PIN_EXTERNAL_LIGHTS, CPin::kAnalog, CPin::kOutput );
}

void CLights::Initialize()
{
	NVehicleManager::m_capabilityBitmask |= ( 1 << LIGHTS_CAPABLE );

	light.Reset();
	light.Write( 0 );

	elight.Reset();
	elight.Write( 0 );
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
		// 0 - 255
		int value = commandIn.m_arguments[1];
		light.Write( value );

		// LIGT - Light toggle
		Serial.print( F( "LIGT:" ) );
		Serial.print( value );
		Serial.print( ';' );

		// LIGP - Light percentage
		Serial.print( F( "LIGP:" ) );
		Serial.print( commandIn.m_arguments[1] / 255.0f );
		Serial.println( ';' );
	}

	if( commandIn.Equals( "eligt" ) )
	{
		float percentvalue = ( float )commandIn.m_arguments[1] / 100.0f; //0 - 255
		int value = 255 * percentvalue;
		elight.Write( value );
		Serial.print( F( "LIGTE:" ) );
		Serial.print( value );
		Serial.print( ';' );
		Serial.print( F( "LIGPE:" ) );
		Serial.print( percentvalue );
		Serial.println( ';' );
	}
}

#endif
