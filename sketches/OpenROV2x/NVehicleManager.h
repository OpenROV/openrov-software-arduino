#pragma once

// Includes
#include <Arduino.h>

// Forward declarations
class CCommand;

namespace NVehicleManager
{
	// TODO: Move
	extern uint32_t m_throttleSmoothingIncrement;
	extern uint32_t m_deadZoneMin;
	extern uint32_t m_deadZoneMax;

	// Methods
	extern void Initialize();
	extern void HandleMessages( CCommand &commandIn );
}
