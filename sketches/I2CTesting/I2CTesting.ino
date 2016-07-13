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
	// Reset the watchdog timer
	NSysManager::ResetWatchdogTimer();
	
	// Handle update loops for each module
	NModuleManager::HandleModuleUpdates( NCommManager::m_currentCommand );
}
