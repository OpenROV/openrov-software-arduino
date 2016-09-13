#pragma once

// Includes
#include "CModule.h"

class CLights : public CModule
{
private:
    uint32_t m_pwmPin;

public:
    CLights( uint32_t pwmPinIn, const char *behaviorsIn, const char* traitsIn );
    
    bool Initialize();
    void Update();
};

