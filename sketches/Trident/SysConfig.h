#pragma once

#include "CompileOptions.h"
#include <CI2C.h>

// TODO: The MCU ARCH and Board Type should eventually be passed in solely from the build script.

// ---------------------------------------------------------
// MCU Architecture Selection
// ---------------------------------------------------------
#define MCUARCH			0
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

#define HAS_STD_LIGHTS 				(1)

// Thrusters configurations
#define THRUSTER_CONFIG_NONE 		(0)
#define THRUSTER_CONFIG_2X1 		(1)

// Selected Thruster Configuration
#define THRUSTER_CONFIGURATION 		THRUSTER_CONFIG_2X1


// ---------------------------------------------------------
// After Market Modules
// ---------------------------------------------------------

// BNO055 IMU
#define HAS_BNO055 					(1)

// MS5837 Depth Sensor
#define HAS_MS5837_30BA				(1)

// MPL3115A2 Alt/Pressure Sensor
#define HAS_MPL3115A2				(1)

// PCA9539 GPIO Expander
#define HAS_PCA9539				(1)

