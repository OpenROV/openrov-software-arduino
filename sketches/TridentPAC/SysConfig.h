#pragma once

#include <Wire.h>

// TODO: The MCU ARCH and Board Type should eventually be passed in solely from the build script.

// ---------------------------------------------------------
// MCU Architecture Selection
// ---------------------------------------------------------
#define MCUARCH					(0)
#define MCUARCH_AVR 			(1)
#define MCUARCH_SAMD 			(2)

// Select
#define MCUARCH					MCUARCH_SAMD

// Controllerboard
#define CONTROLLERBOARD			(0)
#define CONTROLLERBOARD_TRIDENT (1)

// Select
#define CONTROLLERBOARD			CONTROLLERBOARD_TRIDENT

#define SERIAL_INTERFACE0		&Serial
#define SERIAL_INTERFACE1		&Serial1
#define SERIAL_INTERFACE2		&Serial2
#define SERIAL_INTERFACE3		&Serial3
#define SERIAL_INTERFACE4		&Serial4

#define WIRE_INTERFACE_0		&Wire
	
// Thrusters configurations
#define THRUSTER_CONFIG_NONE 		(0)
#define THRUSTER_CONFIG_2X1 		(1)

// Selected Thruster Configuration
#define THRUSTER_CONFIGURATION 		THRUSTER_CONFIG_2X1
