#pragma once

#include <Arduino.h>
#include "CModule.h"

class CMPU9150 : public CModule
{
public:
    virtual void Initialize();
    virtual void Update( CCommand& commandIn );
};