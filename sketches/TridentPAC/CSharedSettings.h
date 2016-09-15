#pragma once

// Includes
#include "CModule.h"

class CVehicleSettings : public CModule
{
public:
    CVehicleSettings( const char *behaviorsIn, const char* traitsIn );
    
    bool Initialize();
    void Update();
};

