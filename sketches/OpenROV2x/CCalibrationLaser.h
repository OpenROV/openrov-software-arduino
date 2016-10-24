#pragma once

// Includes
#include "CModule.h"
#include "CPin.h"

class CCalibrationLaser : public CModule
{
public:
    CCalibrationLaser( uint8_t pinIn );

    void Initialize();
    void Update( CCommand& commandIn );

private:
    CPin m_pin;
};
