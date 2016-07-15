#pragma once

// Includes
#include "CCommand.h"

namespace NCommManager
{
    // Variables
    extern CCommand m_currentCommand;

    extern boolean	m_isCommandAvailable;

    // Methods
    extern void Initialize();
    extern void GetCurrentCommand();
}
