#pragma once

// Includes
#include "CModule.h"

// Defines
#define MAX_MODULES 15

// TODO: CCommand sucks, going to be replaced by a packet format (Mavlink) with intentional message registration and distribution

namespace NModuleManager
{
    // Attributes
    extern int		m_moduleCount;
    extern CModule* m_pModules[ MAX_MODULES ];

    // Methods
    extern void Initialize();
    extern void RegisterModule( CModule* moduleIn );
    extern void HandleModuleUpdates( CCommand& commandIn );
}