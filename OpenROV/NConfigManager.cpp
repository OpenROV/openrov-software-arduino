#include "NConfigManager.h"
#include "NArduinoManager.h"

namespace NConfigManager
{
	// Initialize variables
	uint32_t m_throttleSmoothingIncrement	= 40;
	uint32_t m_deadZoneMin					= 50;
	uint32_t m_deadZoneMax					= 50;
	uint32_t m_capabilityBitmask			= 0;
	uint32_t m_waterType					= WATERTYPE_FRESH;

	void Initialize()
	{
	}

	void HandleMessages( CCommand& commandIn )
	{
		if( commandIn.Equals( "reportSetting" ) )
		{
			Serial.print( F( "*settings:" ) );
			Serial.print( F( "smoothingIncriment|" ) );
			Serial.print( String( NConfigManager::m_throttleSmoothingIncrement ) + "," );
			Serial.print( F( "deadZone_min|" ) );
			Serial.print( String( NConfigManager::m_deadZoneMin ) + "," );
			Serial.print( F( "deadZone_max|" ) );
			Serial.print( String( NConfigManager::m_deadZoneMax ) + "," );
			Serial.print( F( "water_type|" ) );
			Serial.println( String( NConfigManager::m_waterType ) + ";" );

		}
		else if( commandIn.Equals( "rcap" ) ) //report capabilities
		{
			Serial.print( F( "CAPA:" ) );
			Serial.print( m_capabilityBitmask );
			Serial.print( ';' );

			// TODO: Reimplement correctly
			// NArduinoManager::ScanI2C();
		}
		else if( commandIn.Equals( "updateSetting" ) )
		{
			//TODO: Need to update the motors with new deadZone setting. Probably move
			//deadzone to the thruster resposibilitiy
			NConfigManager::m_throttleSmoothingIncrement = commandIn.m_arguments[1];
			NConfigManager::m_deadZoneMin					= commandIn.m_arguments[2];
			NConfigManager::m_deadZoneMax		= commandIn.m_arguments[3];
			NConfigManager::m_waterType		= commandIn.m_arguments[4];
		}
	}
}
