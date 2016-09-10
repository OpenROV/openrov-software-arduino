// Includes
#include <Arduino.h>

#include "CModule.h"
#include "NModuleManager.h"

CModule( const char* behaviorsIn, const char* traitsIn )
	: m_pid( __COUNTER__ )	// Permanently assign unique PID at compile using GCC macro trick
	, m_hasUUID( false )
	, m_uuid( 0 )
	, m_behaviors( behaviorsIn )
	, m_traits( traitsIn )
	, m_regStatus( ERegistrationStatus::UNASSIGNED )
	, m_modStatus( EModuleStatus::UNINITIALIZED )
{
	// Register a pointer to this module in the Module Manager
	NModuleManager::RegisterModule( this );
}

void HandleRegistration()
{
	// If Unregistered
		// Check for ack
		// If ack for me
			// Move to registered
		// Else
			// Send registration request
	// Else
		// Do nothing
}

void HandleInitialization()
{
	// If Uninitialized
		// Call Initialize
		// If Initialize() == true
			// Move to ACTIVE
}

void HandleUpdate()
{
	// If ACTIVE
		// Call Update()
}

void SetUUID( uint32_t uuidIn )
{
	m_uuid = uuidIn;
}										

void ResetModule()
{
	// This will cause the system to move back through its initialization process on the next loop
	m_modStatus = EModuleStatus::UNINITIALIZED;
}		

void DisableModule()
{
	// This will disable the module's Initialize or Update code from ever running
	m_modStatus = EModuleStatus::DISABLED;

	// This will invalidate the module in Cockpit
	DisableRegistration();
}										

void UpdateRegistration( const char* behaviorsIn, const char* traitsIn )
{
	// Reset the module to the unregistered state
	m_regStatus = ERegistrationStatus::UNREGISTERED;

	// Invalidate its UUID
	m_hasUUID = false;

	// Set the new 
}	

void DisableRegistration()
{
	m_regStatus = ERegistrationStatus::DISABLED;
	UpdateRegistration( "disabled", "" );
}											
