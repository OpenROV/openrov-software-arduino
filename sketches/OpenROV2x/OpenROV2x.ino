// Includes
#include <EEPROM.h>

#include "NArduinoManager.h"
#include "NVehicleManager.h"
#include "NDataManager.h"
#include "NCommManager.h"
#include "NModuleManager.h"


void setup()
{
	// Initialize main subsystems
	NArduinoManager::Initialize();
	NCommManager::Initialize();
	NVehicleManager::Initialize();
	NModuleManager::Initialize();
	NDataManager::Initialize();

	// Set timer 5 divisor to 8 for PWM frequency of 3921.16Hz (D44, D45, D46)
	TCCR5B = ( TCCR5B & B11111000 ) | B00000010;

	// Boot complete
	Serial.println( F( "boot:1;" ) );
}

void loop()
{
	// Reset the watchdog timer
	wdt_reset();

	// Attempt to read a current command off of the command line
	NCommManager::GetCurrentCommand();

	// Handle any config change requests
	NVehicleManager::HandleMessages( NCommManager::m_currentCommand );

	// Handle update loops for each module
	NModuleManager::HandleModuleUpdates( NCommManager::m_currentCommand );

	// Handle update loops that send data back to the beaglebone
	NDataManager::HandleOutputLoops();
}
