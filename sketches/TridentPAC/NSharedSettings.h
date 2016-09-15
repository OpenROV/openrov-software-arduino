#pragma once

#include <stdint.h>

// This namespace stores settings shared between several modules, purely as an optimization
namespace NSharedSettings
{
	enum class EWaterType : uint8_t
	{
		FRESH,
		SALT
	}

	extern EWaterType m_waterType;

	// TODO: Remove these and let their modules handle them
	extern uint32_t m_throttleSmoothingIncrement;
	extern uint32_t m_deadZoneMin;
	extern uint32_t m_deadZoneMax;
}
