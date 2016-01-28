#pragma once

// Includes
#include "CModule.h"

class CAutopilot : public CModule
{
public:
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};
