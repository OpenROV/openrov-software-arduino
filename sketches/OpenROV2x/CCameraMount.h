#pragma once

// Includes
#include "CModule.h"

class CCameraMount : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};
