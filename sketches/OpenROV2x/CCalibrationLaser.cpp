#include "SysConfig.h"
#if(HAS_STD_CALIBRATIONLASERS)

// Includes
#include "CCalibrationLaser.h"
#include "NCommManager.h"

CCalibrationLaser::CCalibrationLaser( uint8_t pinIn )
    : m_pin( pinIn, CPin::kAnalog, CPin::kOutput )
{
}

void CCalibrationLaser::Initialize()
{
    m_pin.Write( 0 );
}

void CCalibrationLaser::Update( CCommand& command )
{
    // Check for messages
    if( !NCommManager::m_isCommandAvailable )
    {
        return;
    }

    // Handle messages
    if( command.Equals( "claser" ) )
    {
        // TODO: consistent api with lights

        // Set the laser pin value
        m_pin.Write( command.m_arguments[1] );
        Serial.print( "claser:" ); Serial.print( command.m_arguments[1] ); Serial.println( ';' );
    }
}

#endif



