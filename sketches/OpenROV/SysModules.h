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


// #if(HAS_STD_CAPE)
// #include "CCape.h"
// CCape m_cape;
// #endif

// #if(HAS_STD_LIGHTS)
// #include "CLights.h"
// CLights m_lights;
// #endif

// #if(HAS_STD_CALIBRATIONLASERS)
// #include "CCalibrationLaser.h"
// CalibrationLaser m_calibrationLaser;
// #endif

// #if(THRUSTER_CONFIGURATION != THRUSTER_CONFIG_NONE )
// #include "CThrusters.h"
// CThrusters m_thrusters;
// #endif

// #if(HAS_STD_AUTOPILOT)
// #include "CAutopilot.h"
// CAutopilot m_autopilot;
// #endif

// #if(HAS_STD_CAMERAMOUNT)
// #include "CCameraMount.h"
// CCameraMount m_cameraMount;
// #endif

// #if(HAS_POLOLU_MINIMUV)
// #define COMPASS_ENABLED 1
// #define GYRO_ENABLED 1
// #define ACCELEROMETER_ENABLED 1
// #include "CMinIMU9.h"
// CMinIMU9 m_miniMUV;
// #endif

// #if( HAS_MPU9150 )
// #define COMPASS_ENABLED 1
// #define GYRO_ENABLED 1
// #define ACCELEROMETER_ENABLED 1
// #include "CMPU9150.h"
// CMPU9150 m_mpu9150;
// #endif

// #if(HAS_ALT_SERVO)
// #include "CAltServo.h"
// CAltServo altservo1;
// #endif

#if(HAS_MS5803_30BA)
#include "CMS5803_30BA.h"
CMS5803_30BA m_depthSensor;
#endif

// #if(DEADMANSWITCH_ON)
// #include "CDeadManSwitch.h"
// CDeadManSwitch m_deadManSwitch;
// #endif

#if(HAS_BNO055)
#include "CBNO055.h"
CBNO055 m_boschIMU;
#endif