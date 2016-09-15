// Includes
#include "CModule.h"
#include "NModuleManager.h"
#include "NCommManager.h"

CModule( const char* behaviorsIn, const char* traitsIn )
	: m_hasUUID( false )
	, m_disabling( false )
	, m_uuid( 0 )
	, m_behaviors( behaviorsIn )
	, m_traits( traitsIn )
	, m_regStatus( ERegistrationStatus::UNREGISTERED )
	, m_modStatus( EModuleStatus::UNINITIALIZED )
{
	// Install a pointer to this module in the Module Manager
	NModuleManager::InstallModule( this );
}

void HandleRegistration()
{
	if( NCommManager::m_isCommandAvailable )
	{
		// Check for ack message that matches this module's UUID
		// TODO: Work out new CCommand with built in support for filtering on UUID checking
		if( NCommManager::m_currentCommand.Equals( "ack" ) && NCommManager::m_currentCommand.m_arguments[1] == m_uuid )
		{
			if( m_disabling )
			{
				m_regStatus = ERegistrationStatus::DISABLED;
			}
			else
			{
				m_regStatus = ERegistrationStatus::REGISTERED;
			}
		}
	}
	else
	{
		// Send registration request
		Serial.print( "reg:" );
		Serial.print( m_pid );			Serial.print( "|" );
		Serial.print( m_uuid );			Serial.print( "|" );
		Serial.print( m_behaviors );	Serial.print( "|" );
		Serial.print( m_traits );		Serial.println( ";" );
	}
}

void HandleUpdate()
{
	if( m_modStatus == EModuleStatus::UNINITIALIZED )
	{
		// Call initialize function until module is initialized
		if( Initialize() == true )
		{
			// Move to active mode
			m_modStatus = EModuleStatus::ACTIVE;
		}
	}	
	else if( m_modStatus == EModuleStatus::ACTIVE )
	{
		// Once in active state, call module's update function
		Update();
	}
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

	// Set the new behaviors and traits
	m_behaviors = behaviorsIn;
	m_traits 	= traitsIn;
}	

void DisableRegistration()
{
	UpdateRegistration( "disabled", "" );

	// After the registration update is acked, the registration status will be moved to DISABLED
	m_disabling = true;
}											
