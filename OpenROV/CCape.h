#pragma once
// Includes
#include "CModule.h"

#define LIGHTS_PIN				5
#define CAMERAMOUNT_PIN			3
#define CALIBRATIONLASERS_PIN	6
#define CAPE_VOLTAGE_PIN		0
#define CAPE_CURRENT_PIN		3
#define PORT_PIN				9
#define VERTICAL_PIN			10
#define STARBOARD_PIN			11

class CCape : public CModule
{
private:

public:
	void Initialize();
	void Update( CCommand& commandIn );
};
