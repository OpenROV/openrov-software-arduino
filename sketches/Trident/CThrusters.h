#pragma once

// Includes
#include "CModule.h"

class CThrusters : public CModule
{
public:
    static const int kMotorCount;

    void Initialize();
    void Update( CCommand& commandIn );
};