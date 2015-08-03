#pragma once

// Includes
#include "CModule.h"

class CBNO055 : public CModule
{
public:
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};
