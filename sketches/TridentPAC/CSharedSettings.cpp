#include "CSharedSettings.h"
#include "NSharedSettings.h"

bool CSharedSettings::Initialize()
{
    // Reset to defaults
    NSharedSettings::m_waterType                    = EWaterType::FRESH;
    NSharedSettings::m_throttleSmoothingIncrement	= 40;
	NSharedSettings::m_deadZoneMin					= 50;
	NSharedSettings::m_deadZoneMax					= 50;

	return true;
}

void CSharedSettings::Update()
{
    if( !NCommManager::MessageAvailable( m_uuid ) )
    {
        return;
    }

    bool sendState = false;

    // Handle behavior specific messages
	if( NCommManager::m_currentCommand.IsBehavior( "aquatic" ) )
	{
		if( NCommManager::m_currentCommand.IsCommand( "watertype" ) )
		{
            NSharedSettings::m_waterType = commandIn.m_arguments[2];
            sendState = true;
        }

        if( NCommManager::m_currentCommand.IsCommand( "query" ) )
		{
            sendState = true;
        }
    }
    elseif( NCommManager::m_currentCommand.IsBehavior( "rov" ) )
	{
		if( NCommManager::m_currentCommand.IsCommand( "ctrlSettings" ) )
		{
            NSharedSettings::m_throttleSmoothingIncrement	= commandIn.m_arguments[2];
            NSharedSettings::m_deadZoneMin					= commandIn.m_arguments[3];
            NSharedSettings::m_deadZoneMax					= commandIn.m_arguments[4];

            sendState = true;
        }
        else if( NCommManager::m_currentCommand.IsCommand( "query" ) )
		{
            sendState = true;
        }
    }

    if( sendState )
    {
        // Print telemetry
        Serial.print( m_uuid ); 	
        Serial.print( ":aquatic|watertype=" );    Serial.print( m_waterType );
        Serial.println( ";" );
    }

    // If message available
        // If IsBehavior( watervehicle )
            // If Is( Update )
                // Update state
            // If Is( Query )
                // Send current state
}