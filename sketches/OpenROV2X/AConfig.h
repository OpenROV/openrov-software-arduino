#pragma once

/* This section is for modules and their configuration. IF you have not setup you pins with the
*  standard configuration of the OpenROV kits, you should probably clone the cape or controlboard
*  and change the pin definitions there.  Things not wired to specific pins but on the I2C bus will
*  have the address defined in this file.
*/

#include "BoardConfig.h"

// Kit:
// The boards are auto configure in the on-beaglebone build process in the BoardConfig.h which gets
// created. If running directly in an IDE, the BoardConfig.h wont exist and the #defines below will be
// used instead.

// ---------------------------------------------------------
// Standard Modules
// ---------------------------------------------------------

// Choose a cape or controller board
#define HAS_STD_CAPE (0)
#define HAS_OROV_CONTROLLERBOARD_25 (1)

#if !(HAS_OROV_CONTROLLERBOARD_25) && !(HAS_STD_CAPE)
#  error "You must select either standard cape or controllerboard25 in the AConfig.h file as they have predefined pin values required by other libraries."
#endif

#define HAS_STD_LIGHTS (1)
#define HAS_EXT_LIGHTS (1)
#define HAS_STD_CALIBRATIONLASERS (1)
#define HAS_STD_CAMERAMOUNT (1)
#define HAS_STD_AUTOPILOT (1)
#define HAS_EXP_AUTOPILOT (0)
#define HAS_ALT_SERVO (1)
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

// Pololu MINIMUV
#define HAS_POLOLU_MINIMUV (0)

// MS5803_XXBA Depth Sensor
#define HAS_MS5803_XXBA (0)
#define MS5803_XXBA_I2C_ADDRESS 0x76

// MS5837_30BA Depth Sensor
#define HAS_MS5837_30BA (1)
#define MS5837_30BA_I2C_ADDRESS 0x76

// MPU9150 IMU
#define HAS_MPU9150 (1)
#define HAS_BNO055 (1)
#define MPU9150_EEPROM_START 2
#define HAS_ALT_SERVO (1)
#define ALTS_MIDPOINT 1500
#define ALTS_MINPOINT 1000
#define ALTS_MAXPOINT 2000

// BNO055 IMU
#define HAS_BNO055 (1)
