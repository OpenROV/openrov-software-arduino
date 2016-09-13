// Includes
#include <Arduino.h>
#include "NModuleManager.h"
#include "CModule.h"
#include "CTimer.h"

// This file instantiates any required core modules
#include "SysModules.h"

namespace
{
	// Private Attributes
	constexpr uint32_t MAX_MODULES = 15;
	constexpr uint32_t REGISTRATION_DELAY_MS = 30;

	uint32_t m_uuidCounter 	= 0;
	uint32_t m_moduleCount 	= 0;

	CTimer m_regTimer;

	CModule *m_pModules[ MAX_MODULES ];
}

// Public
namespace NModuleManager
{
	void Initialize()
	{
		m_regTimer.Reset();
	}

	void InstallModule( CModule *moduleIn )
	{
		// Check to see if we have room left to register a module
		if( m_moduleCount >= MAX_MODULES )
		{
			return;
		}

		// Set module pointer
		m_pModules[ m_moduleCount ] = moduleIn;

		// Increment module count
		++m_moduleCount;
	}

	void HandleModuleUpdates()
	{
		for( uint32_t i = 0; i < m_moduleCount; ++i )
		{
			// Check to see if the module has been issued a UUID
			if( m_pModules[ i ]->HasUUID() == false )
			{
				// Assign new one, bump the counter
				m_pModules[ i ]->SetUUID( m_uuidCounter++ );
			}

			// Throttle registrations to avoid overuse of comm channel bandwidth
			if( m_regTimer.HasElapsed( REGISTRATION_DELAY_MS ) )
			{
				if( m_pModules[ i ]->IsUnregistered() )
				{
					// Let the module handle registration logic
					m_pModules[ i ]->HandleRegistration();
				}
			}

			// Call the module's implemented Initialize() and Update() methods
			m_pModules[ i ]->HandleUpdate();
		}
	}
}