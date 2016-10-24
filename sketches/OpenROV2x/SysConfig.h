#pragma once

#include "PinDefinitions.h"
#include "CompileOptions.h"

// TODO: The MCU ARCH and Board Type should eventually be passed in solely from the build script.

// ---------------------------------------------------------
// MCU Architecture Selection
// ---------------------------------------------------------

#ifndef ARDUINO_ARCH_AVR
	#error "This sketch is only supported on for ARDUINO_ARCH_AVR!"
#endif

// Configuration
#define HAS_OROV_CONTROLLERBOARD_25 (1)
#define HAS_STD_LIGHTS (1)
#define HAS_STD_CALIBRATIONLASERS (1)
#define HAS_CAMERASERVO (1)
#define HAS_STD_AUTOPILOT (1)
#define DEADMANSWITCH_ON (0)

// Thrusters configurations  (These appear depricated and replaced by the THRUSTER_CONFIGURATION below)
#define THRUSTER_CONFIG_NONE (0)
#define THRUSTER_CONFIG_2X1 (1)
#define THRUSTER_CONFIG_2Xv2 (2)
#define THRUSTER_CONFIG_v2X1Xv2 (3)

// Selected Thruster Configuration
#define THRUSTER_CONFIGURATION THRUSTER_CONFIG_2X1
//#define THRUSTER_CONFIGURATION THRUSTER_CONFIG_2Xv2
//#define THRUSTER_CONFIGURATION THRUSTER_CONFIG_v2X1Xv2

// ---------------------------------------------------------
// After Market Modules
// ---------------------------------------------------------

// External Lights
#define HAS_EXT_LIGHTS (1)

// AltServo
#define HAS_ALT_SERVO (0)

// MS5803_14BA Depth Sensor
#define HAS_MS5803_14BA (1)

// MS5837_30BA Depth Sensor
#define HAS_MS5837_30BA (1)

// MPU9150
#define HAS_MPU9150 (1)

// BNO055 IMU
#define HAS_BNO055 (1)