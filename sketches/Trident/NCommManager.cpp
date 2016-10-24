#include "NCommManager.h"

namespace NCommManager
{
	// ---------------------------------------------------------
	// Variable initialization
	// ---------------------------------------------------------

	CCommand	m_currentCommand;
	bool		m_isCommandAvailable = false;

	// ---------------------------------------------------------
	// Method Definitions
	// ---------------------------------------------------------

	void Initialize()
	{
		Serial.println( "Systems.CommManager.Status:INIT;" );

		// TODO: Mavlink stuff will probably happen here

		Serial.println( "Systems.CommManager.Status:READY;" );
	}

	void GetCurrentCommand()
	{
		// Get the current command string from the serial port
		m_isCommandAvailable = m_currentCommand.GetCommandString();
	}
}
