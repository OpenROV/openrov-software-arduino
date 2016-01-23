// Includes
#include "NSysManager.h"
#include "NCommManager.h"
#include "NVehicleManager.h"
#include "NModuleManager.h"
#include "NDataManager.h"

void setup()
{
	// Initialize main subsystems
	NSysManager::Initialize();
	NCommManager::Initialize();
	NVehicleManager::Initialize();
	NModuleManager::Initialize();
	NDataManager::Initialize();

	// Boot complete
	Serial.println( "bootstage:1;" );
}

void loop()
{
	// // Reset the watchdog timer
	NSysManager::ResetWatchdogTimer();

	// Attempt to read a current command off of the command line
	NCommManager::GetCurrentCommand();

	// Handle any vehicle information requests or updates
	NVehicleManager::HandleMessages( NCommManager::m_currentCommand );

	// Handle update loops for each module
	NModuleManager::HandleModuleUpdates( NCommManager::m_currentCommand );

	// Handle update loops that send data back to the beaglebone
	NDataManager::HandleOutputLoops();
}
