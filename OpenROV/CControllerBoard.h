#pragma once

// Includes
#include "CModule.h"

class CControllerBoard : public CModule
{
public:
    // Attributes

    // Methods
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};