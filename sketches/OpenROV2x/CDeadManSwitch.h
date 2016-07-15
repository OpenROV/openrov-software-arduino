#pragma once

// Includes
#include "CModule.h"

class CDeadManSwitch : public CModule
{
public:
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};
