#pragma once

// Includes
#include "CModule.h"

#error "This code is obsolete and exists only as a reference. Update it to the latest cockpit APIs to use it."

class CCameraServo : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};
