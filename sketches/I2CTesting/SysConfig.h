#pragma once

#include <I2C.h>

// TODO: The MCU ARCH and Board Type should eventually be passed in solely from the build script.

// ---------------------------------------------------------
// MCU Architecture Selection
// ---------------------------------------------------------
#define MCUARCH			0
#define MCUARCH_AVR 	1
#define MCUARCH_SAMD 	2

// Select here
#define MCUARCH			MCUARCH_SAMD

#if MCUARCH == 0
	#error "SysConfig.h: You must select an MCU architecture!"
#endif

// ---------------------------------------------------------
// Controller Board Selection
// ---------------------------------------------------------
#define CONTROLLERBOARD					0
#define CONTROLLERBOARD_CB25			2
#define CONTROLLERBOARD_TRIDENT			3

// Select here
#define CONTROLLERBOARD					CONTROLLERBOARD_TRIDENT

#if CONTROLLERBOARD == 0
	#error "SysConfig.h: You must select a board type!"
#endif

// ---------------------------------------------------------
// Peripheral Interfaces
// ---------------------------------------------------------

#if (MCUARCH == MCUARCH_SAMD)

	#if WIRE_INTERFACES_COUNT > 0
		#define WIRE_INTERFACE_0	&I2C0
	#else
		#define WIRE_INTERFACE_0	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 1
		#define WIRE_INTERFACE_1	&I2C1
	#else
		#define WIRE_INTERFACE_1	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 2
		#define WIRE_INTERFACE_2	&I2C2
	#else
		#define WIRE_INTERFACE_2	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 3
		#define WIRE_INTERFACE_3	&I2C3
	#else
		#define WIRE_INTERFACE_3	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 4
		#define WIRE_INTERFACE_4	&I2C4
	#else
		#define WIRE_INTERFACE_4	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 5
		#define WIRE_INTERFACE_5	&I2C5
	#else
		#define WIRE_INTERFACE_5	nullptr
	#endif

#else
	#define WIRE_INTERFACES_COUNT 	1
	#define WIRE_INTERFACE_0		&I2C0
	#define WIRE_INTERFACE_1		nullptr
	#define WIRE_INTERFACE_2		nullptr
	#define WIRE_INTERFACE_3		nullptr
	#define WIRE_INTERFACE_4		nullptr
	#define WIRE_INTERFACE_5		nullptr
#endif

// ---------------------------------------------------------
// Standard Modules
// ---------------------------------------------------------

#define HAS_STD_LIGHTS 				(0)
#define HAS_STD_CALIBRATIONLASERS 	(0)
#define HAS_STD_CAMERAMOUNT 		(0)
#define HAS_STD_AUTOPILOT 			(0)
#define HAS_ALT_SERVO 				(0)
#define HAS_DEADMAN_SWITCH 			(0)

// Thrusters configurations
#define THRUSTER_CONFIG_NONE 		(0)
#define THRUSTER_CONFIG_2X1 		(1)
#define THRUSTER_CONFIG_2XV2 		(2)
#define THRUSTER_CONFIG_V2X1XV2 	(3)

// Selected Thruster Configuration
#define THRUSTER_CONFIGURATION 		THRUSTER_CONFIG_NONE


// ---------------------------------------------------------
// After Market Modules
// ---------------------------------------------------------

// Todo: These will eventually become standalone plugins!

// MPU9150 IMU
#define HAS_MPU9150 				(0)
#define MPU9150_EEPROM_START 		(2)

// BNO055 IMU
#define HAS_BNO055 					(0)

// MS5803_XXBA Depth Sensor
#define HAS_MS5803					(0)
#define HAS_MS5803_30BA 			(0)
#define HAS_MS5803_14BA				(0)




