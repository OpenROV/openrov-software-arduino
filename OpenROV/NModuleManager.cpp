// Includes
#include <avr/wdt.h>

#include "NModuleManager.h"
#include "ModuleDefinitions.h"

namespace NModuleManager
{
    // Initialize variables
    int	m_moduleCount{ 0 };
    CModule* m_pModules[ MAX_MODULES ];

    // Method definitions
    void Initialize()
    {
        for( int i = 0; i < m_moduleCount; ++i )
        {
            // Each module setup can take up to a full second or so to init.
            // Reset the WDT after each init to keep it from triggering during this step.
            wdt_reset();

            m_pModules[ i ]->Initialize();
        }
    }

    void RegisterModule( CModule* moduleIn )
    {
        // Check to see if we have room left to register a module
        if( m_moduleCount >= MAX_MODULES )
        {
            Serial.println( F( "ERROR: TOO MANY MODULES;" ) );
            return;
        }

        // Set module pointer
        m_pModules[ m_moduleCount ] = moduleIn;

        // Increment module count
        ++m_moduleCount;
    }

    void HandleModuleUpdates( CCommand& commandIn )
    {
        // Loop through each module and call its update functon
        for( int i = 0; i < m_moduleCount; ++i )
        {
            m_pModules[ i ]->Update( commandIn );

            // MOD: Loop counters were removed because they are not used and have no discernible purpose that giving a module a CTimer can't solve.
        }
    }
}