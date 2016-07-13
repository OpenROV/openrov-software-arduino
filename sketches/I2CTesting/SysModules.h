#pragma once

#include "SysConfig.h"

// ---------------------------------------
// Conditional module definitions
// -------------------------------
// Once these objects are instantiated, they will register themselves as modules in the module manager.
// They can also be accessed individually as namespace members.
// ---------------------------------------

#if(HAS_MS5837_30BA)
#include "CMS5837_30BA.h"
CMS5837_30BA m_depthSensor;
#endif
