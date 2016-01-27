#include "SysConfig.h"
#if(HAS_STD_CALIBRATIONLASERS)

// Includes
#include "CCalibrationLaser.h"
#include "CPin.h"
#include "NCommManager.h"
#include "NVehicleManager.h"
#include "PinDefinitions.h"

// File local variables and methods
namespace
{
    CPin calLaser( "claser", PIN_LASERS, CPin::kAnalog, CPin::kOutput );
}

void CalibrationLaser::Initialize()
{
    NVehicleManager::m_capabilityBitmask |= ( 1 << CALIBRATION_LASERS_CAPABLE );
    calLaser.Write( 0 );
}

void CalibrationLaser::Update( CCommand& command )
{
    // Check for messages
    if( !NCommManager::m_isCommandAvailable )
    {
        return;
    }

    // Handle messages
    if( command.Equals( "claser" ) )
    {
        int value = command.m_arguments[1];

        // Set the laser pin value
        calLaser.Write( value );
        calLaser.Send( value );
    }
}

#endif



