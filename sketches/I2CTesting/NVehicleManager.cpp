// Includes
#include "NVehicleManager.h"
#include "CCommand.h"

namespace NVehicleManager
{
	// ---------------------------------------------------------
	// Variable initialization
	// ---------------------------------------------------------

	uint32_t m_throttleSmoothingIncrement	= 40;
	uint32_t m_deadZoneMin					= 50;
	uint32_t m_deadZoneMax					= 50;
	uint32_t m_capabilityBitmask			= 0;
	uint32_t m_waterType					= WATERTYPE_FRESH;

	// ---------------------------------------------------------
	// Method Definitions
	// ---------------------------------------------------------

	void Initialize()
	{
		Serial.println( "Systems.VehicleManager.Status:INIT;" );

		Serial.println( "Systems.VehicleManager.Status:READY;" );
	}

	void HandleMessages( CCommand &commandIn )
	{
		
	}
}
