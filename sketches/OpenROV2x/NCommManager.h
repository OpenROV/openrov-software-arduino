#pragma once

// Includes
#include "CCommand.h"

namespace NCommManager
{
    // Variables
    extern CCommand m_currentCommand;

    extern bool	m_isCommandAvailable;

    // Methods
    extern void Initialize();
    extern void GetCurrentCommand();
}
