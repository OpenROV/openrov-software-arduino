#pragma once

// Includes
#include "SysConfig.h"
#include "CModule.h"
#include "PinDefinitions.h"

class CControllerBoard : public CModule
{
public:
    // Methods
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};
