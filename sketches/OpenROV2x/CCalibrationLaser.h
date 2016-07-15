#pragma once

// Includes
#include "CModule.h"

class CalibrationLaser : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};
