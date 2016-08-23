// Includes
#include "NModuleManager.h"
#include "NSysManager.h"

// This file instantiates any required core modules
#include "SysModules.h"

namespace NModuleManager
{
	// Initialize variables
	int	m_moduleCount = 0 ;
	CModule *m_pModules[ MAX_MODULES ];

	// Method definitions
	void Initialize()
	{
		Serial.println( "Systems.ModuleManager.Status:INIT;" );

		Serial.print( "log:Module Count=" );
		Serial.print( m_moduleCount );
		Serial.println( ";" );

		for( int i = 0; i < m_moduleCount; ++i )
		{
			m_pModules[ i ]->Initialize();

			Serial.print( "log:Module Init=" );
			Serial.print( m_pModules[ i ]->m_name );
			Serial.println( ";" );
		}

		Serial.println( "Systems.ModuleManager.Status:READY;" );
	}

	void RegisterModule( CModule *moduleIn )
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

	void HandleModuleUpdates( CCommand &commandIn )
	{
		unsigned long start = 0;
		unsigned long stop	= 0;

		// Loop through each module and call its update functon
		for( int i = 0; i < m_moduleCount; ++i )
		{
			// Time and execute the update functions for this module
			start = millis();
			m_pModules[ i ]->Update( commandIn );
			stop = millis();

			// Set the delta for the module's execution time so we can send this out as debug info
			m_pModules[i]->m_executionTime = stop - start;
		}
	}
}