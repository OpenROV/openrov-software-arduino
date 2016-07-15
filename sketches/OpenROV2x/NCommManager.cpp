#include "NCommManager.h"

namespace NCommManager
{
    // Initialize variables
    CCommand	m_currentCommand;
    boolean		m_isCommandAvailable = false;

    void Initialize()
    {
    }

    void GetCurrentCommand()
    {
        // Get the current command string from the serial port
        m_isCommandAvailable = m_currentCommand.GetCommandString();
    }
}
