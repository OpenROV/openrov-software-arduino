#pragma once

#include "SystemConstants.h"
#include "CompileOptions.h"

// TODO: The MCU ARCH and Board Type should eventually be passed in solely from the build script.

// ---------------------------------------------------------
// MCU Architecture Selection
// ---------------------------------------------------------
#define MCUARCH			0
#define MCUARCH_AVR 	1
#define MCUARCH_SAMD 	2

// Select here
#define MCUARCH			MCUARCH_AVR

#if MCUARCH == 0
	#error "SysConfig.h: You must select an MCU architecture!"
#endif

// ---------------------------------------------------------
// Controller Board Selection
// ---------------------------------------------------------
#define CONTROLLERBOARD					0
#define CONTROLLERBOARD_CAPE 			1
#define CONTROLLERBOARD_CB25			2
#define CONTROLLERBOARD_TRIDENT			3

// Select here
#define CONTROLLERBOARD					CONTROLLERBOARD_CB25

#if CONTROLLERBOARD == 0
	#error "SysConfig.h: You must select a board type!"
#endif

#define HAS_EXT_LIGHTS (1)