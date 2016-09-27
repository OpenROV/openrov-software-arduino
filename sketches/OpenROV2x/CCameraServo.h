#pragma once

// Includes
#include "CModule.h"

class CCameraServo : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};
