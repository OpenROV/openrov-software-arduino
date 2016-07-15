#pragma once

// Includes
#include <Arduino.h>
#include "CCommand.h"

// If you have modules that add capabilities that should be advertised to the OpenROV Cockpit
// software add them here. Check the bits on the server side for capabilities and gracefully
// degrade (remove UI elements) if the capabilities do not exist.
#define LIGHTS_CAPABLE					1
#define CALIBRATION_LASERS_CAPABLE		2
#define CAMERA_MOUNT_1_AXIS_CAPABLE		3
#define COMPASS_CAPABLE					4
#define ORIENTATION_CAPABLE				5
#define DEPTH_CAPABLE					6
#define ALT_SERVO_CAPABLE				7

namespace NConfigManager
{
	// Variables
	extern uint32_t m_throttleSmoothingIncrement;	// How aggressively the throttle changes
	extern uint32_t m_deadZoneMin;
	extern uint32_t m_deadZoneMax;
	extern uint32_t m_capabilityBitmask;
	extern uint32_t m_waterType;

	// Methods
	extern void Initialize();
	extern void HandleMessages( CCommand& commandIn );
}
