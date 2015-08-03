#include "AConfig.h"
#if(HAS_STD_LIGHTS)

// Includes
#include <Arduino.h>
#include "CLights.h"
#include "CPin.h"
#include "NConfigManager.h"
#include "NModuleManager.h"
#include "NCommManager.h"

// One of these will have the correct pin defined for the lights
#if(HAS_STD_CAPE)
    #include "CCape.h"
#endif

#if(HAS_OROV_CONTROLLERBOARD_25)
    #include "CControllerBoard.h"
#endif

namespace
{
    CPin light( "light", LIGHTS_PIN, CPin::kAnalog, CPin::kOutput );
}

void CLights::Initialize()
{
    NConfigManager::m_capabilityBitmask |= ( 1 << LIGHTS_CAPABLE );

    light.Reset();
    light.Write( 0 );
}

void CLights::Update( CCommand& command )
{
    // Check for messages
    if( !NCommManager::m_isCommandAvailable )
    {
        return;
    }

    // Handle messages
    if( command.Equals( "ligt" ) )
    {
        // 0 - 255
        int value = command.m_arguments[1];
        light.Write( value );

        // LIGT - Light toggle
        Serial.print( F( "LIGT:" ) );
        Serial.print( value );
        Serial.print( ';' );

        // LIGP - Light percentage
        Serial.print( F( "LIGP:" ) );
        Serial.print( command.m_arguments[1] / 255.0 );
        Serial.println( ';' );
    }
}

#endif
