#pragma once

// Includes
#include "CModule.h"
#include "CPin.h"

class CLights : public CModule
{
public:
    CLights( uint32_t pwmPinIn, const char *behaviorsIn, const char* traitsIn );
    
    bool Initialize();
    void Update();

private:
    CPin        m_pwmPin;
    uint32_t    m_power;
};

