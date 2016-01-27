#pragma once

// Includes
#include "CModule.h"

class CLights : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};

