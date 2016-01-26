#pragma once

#include "SysConfig.h"
// Provide the correct pin definitions header based on board type

#if CONTROLLERBOARD == CONTROLLERBOARD_TRIDENT
	#include "PinDefinitions_Trident.h"
#elif CONTROLLERBOARD == CONTROLLERBOARD_25
	#include "PinDefinitions_25.h"
#elif CONTROLLERBOARD == CONTROLLERBOARD_CAPE
	#include "PinDefinitions_Cape.h"
#endif