#pragma once

// Includes
#include "CCommand.h"

namespace NCommManager
{
	enum class ERetCode
	{
		SUCCESS,				// Message successfully inserted in outbound buffer
		OVERSIZED_MESSAGE,		// Message too large to send. Consider shortening or breaking into multiple messages
		TX_BUFFER_FULL			// Currently no space in outbound buffer
	};

	// Variables
	extern CCommand m_currentCommand;

	extern boolean	m_isCommandAvailable;

	// Methods
	extern void Initialize();
	extern void GetCurrentCommand();

	// TODO: Some kind of magic template function that lets Module devs easily send data across the UART
	// template<class... TArgs>
	// ERetCode SendMessage( uint32_t uuidIn, const char* behaviorIn, TArgs... args);
}
