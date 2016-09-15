// Includes
#include "NSharedSettings.h"

namespace NSharedSettings
{
	// Set defaults
	EWaterType m_waterType = EWaterType::FRESH;

	uint32_t m_throttleSmoothingIncrement	= 40;
	uint32_t m_deadZoneMin					= 50;
	uint32_t m_deadZoneMax					= 50;
}
