#pragma once

#include "SysConfig.h"

// ---------------------------------------
// Conditional module definitions
// -------------------------------
// Once these objects are instantiated, they will register themselves as modules in the module manager.
// They can also be accessed individually as namespace members.
// ---------------------------------------

// Must have one of these!
#include "CControllerBoard.h"
CControllerBoard m_controllerBoard;

#include "Plugins.h"

#if(HAS_STD_LIGHTS)
#include "CLights.h"
CLights m_lights;
#endif

#if(THRUSTER_CONFIGURATION != THRUSTER_CONFIG_NONE )
#include "CThrusters.h"
CThrusters m_thrusters;
#endif

#if(HAS_BNO055)
#include "CBNO055.h"
CBNO055 m_boschIMU( WIRE_INTERFACE_0 );
#endif

#if(HAS_MS5837_30BA)
#include "CMS5837_30BA.h"
CMS5837_30BA m_depthSensor( WIRE_INTERFACE_0 );
#endif

#if(HAS_BMP280)
#include "CBMP280.h"
CBMP280 m_bmp280( WIRE_INTERFACE_0 );
#endif