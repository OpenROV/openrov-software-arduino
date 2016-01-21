#pragma once

#include <Wire.h>

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
	#error "Config.h: You must select an MCU architecture!"
#endif

// ---------------------------------------------------------
// Controller Board Selection
// ---------------------------------------------------------
#define BOARDTYPE				0
#define BOARDTYPE_CAPE 			1
#define BOARDTYPE_CB25			2
#define BOARDTYPE_TRIDENT_ALPHA 3

// Select here
#define BOARDTYPE				BOARDTYPE_TRIDENT_ALPHA

#if BOARDTYPE == 0
	#error "Config.h: You must select a board type!"
#endif

//
// Peripheral Interfaces
//
	
	#if WIRE_INTERFACES_COUNT > 0
	  	#define WIRE_INTERFACE_0	&Wire
	#else
		#define WIRE_INTERFACE_0	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 1
	  	#define WIRE_INTERFACE_1	&Wire1
  	#else
		#define WIRE_INTERFACE_1	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 2
 		#define WIRE_INTERFACE_2	&Wire2
 	#else
		#define WIRE_INTERFACE_2	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 3
		#define WIRE_INTERFACE_3	&Wire3
	#else
		#define WIRE_INTERFACE_3	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 4
		#define WIRE_INTERFACE_4	&Wire4
	#else
		#define WIRE_INTERFACE_4	nullptr
	#endif
	#if WIRE_INTERFACES_COUNT > 5
		#define WIRE_INTERFACE_5	&Wire5
	#else
		#define WIRE_INTERFACE_5	nullptr
	#endif

// ---------------------------------------------------------
// Standard Modules
// ---------------------------------------------------------

// Choose a cape or controller board
// #define HAS_STD_CAPE (0)
// #define HAS_OROV_CONTROLLERBOARD_25 (1)

// #if !(HAS_OROV_CONTROLLERBOARD_25) && !(HAS_STD_CAPE)
// #  error "You must select either standard cape or controllerboard25 in the AConfig.h file as they have predefined pin values required by other libraries."
// #endif

// #define HAS_STD_LIGHTS (1)
// #define HAS_STD_CALIBRATIONLASERS (1)
// #define HAS_STD_CAMERAMOUNT (1)
// #define HAS_STD_AUTOPILOT (1)
// #define HAS_EXP_AUTOPILOT (0)
// #define HAS_ALT_SERVO (1)
// #define DEADMANSWITCH_ON (1)

// // Thrusters configurations
// #define THRUSTER_CONFIG_NONE (0)
// #define THRUSTER_CONFIG_2X1 (1)

// // Selected Thruster Configuration
// #define THRUSTER_CONFIGURATION THRUSTER_CONFIG_2X1


// // ---------------------------------------------------------
// // After Market Modules
// // ---------------------------------------------------------

// // Pololu MINIMUV
// #define HAS_POLOLU_MINIMUV (0)

// // MS5803_XXBA Depth Sensor
// #define HAS_MS5803_XXBA (1)
// #define MS5803_XXBA_I2C_ADDRESS 0x76

// // MPU9150 IMU
// #define HAS_MPU9150 (1)
// #define HAS_BNO055 (1)
// #define MPU9150_EEPROM_START 2
// #define HAS_ALT_SERVO (1)
// #define ALTS_MIDPOINT 1500
// #define ALTS_MINPOINT 1000
// #define ALTS_MAXPOINT 2000

// // BNO055 IMU
#define HAS_BNO055 (1)
