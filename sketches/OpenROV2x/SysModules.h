#pragma once
#include "SysConfig.h"

#include "Plugins.h"
#include "PinDefinitions.h"



// ---------------------------------------
// Conditional module definitions
// -------------------------------
// Once these objects are instantiated, they will register themselves as modules in the module manager.
// They can also be accessed individually as namespace members.
// ---------------------------------------

#if( HAS_OROV_CONTROLLERBOARD_25 )
#include "CControllerBoard.h"
CControllerBoard m_controllerBoard;
#endif

#if(HAS_STD_LIGHTS)
#include "CLights.h"
CLights m_lights( PIN_STANDARD_LIGHTS );
#endif

#if(HAS_EXT_LIGHTS )
#include "CExternalLights.h"
CExternalLights m_elights( PIN_PWM_3 );
#endif

#if(HAS_STD_CALIBRATIONLASERS)
#include "CCalibrationLaser.h"
CCalibrationLaser m_calibrationLaser( PIN_LASERS );
#endif

#if(THRUSTER_CONFIGURATION != THRUSTER_CONFIG_NONE )
#include "CThrusters.h"
CThrusters m_thrusters;
#endif

#if(HAS_STD_AUTOPILOT)
#include "CAutopilot.h"
CAutopilot m_autopilot;
#endif

#if(HAS_CAMERASERVO)
#include "CCameraServo.h"
CCameraServo m_cameraServo;
#endif

#if(HAS_ALT_SERVO)
#include "CAltServo.h"
CAltServo altservo1;
#endif

#if(DEADMANSWITCH_ON)
#include "CDeadManSwitch.h"
CDeadManSwitch m_deadManSwitch;
#endif

// IMU1
#if( HAS_MPU9150 )
#include "CMPU9150.h"
CMPU9150 m_mpu9150( mpu9150::EAddress::ADDRESS_B );
#endif

#if(HAS_MS5803_14BA)
#include "CMS5803_14BA.h"
CMS5803_14BA m_ms5803( &I2C0, ms5803_14ba::EAddress::ADDRESS_A );
#endif

// IMU2
#if(HAS_BNO055)
#include "CBNO055.h"
CBNO055 m_bno055( &I2C0, bno055::EAddress::ADDRESS_A );
#endif

#if(HAS_MS5837_30BA)
#include "CMS5837_30BA.h"
CMS5837_30BA m_ms5837( &I2C0, ms5837_30ba::EAddress::ADDRESS_A );
#endif


