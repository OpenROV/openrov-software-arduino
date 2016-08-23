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

#include "CTestESC.h"
CTestESC m_esc;

// #if(THRUSTER_CONFIGURATION != THRUSTER_CONFIG_NONE )
// #include "CThrusters.h"
// CThrusters m_thrusters;
// #endif