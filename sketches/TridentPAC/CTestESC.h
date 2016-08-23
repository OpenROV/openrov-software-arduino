#pragma once

// Includes
#include "CModule.h"

class CTestESC : public CModule
{
public:
    void Initialize();
    void Update( CCommand& commandIn );
};