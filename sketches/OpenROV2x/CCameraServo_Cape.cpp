#include "AConfig.h"
#if(HAS_CAMERASERVO_CAPE)

#error "This code is obsolete and exists only as a reference. Update it to the latest cockpit APIs to use it."

// Includes
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "CCameraServo_Cape.h"
#include "CServo.h"
#include "CPin.h"
#include "NConfigManager.h"
#include "NDataManager.h"
#include "NModuleManager.h"
#include "NCommManager.h"
#include "CCape.h"

// File local variables and methods
namespace
{
    CServo tilt;

    int commandedTilt	= CAMERA_SERVO_TARGET_MIDPOINT_US;
    int tiltTarget		= CAMERA_SERVO_TARGET_MIDPOINT_US;

    const int tiltRate	= 1;

    int m_isInverted    = 0;    // 0 - Not inverted, 1 - Inverted

    int SmoothlyAdjustCameraPosition( int target, int current )
    {
        int x			= target - current;
        int sign		= ( x > 0 ) - ( x < 0 );
        int adjustedVal = current + sign * ( min( abs( target - current ), tiltRate ) );

        return adjustedVal;
    }

    void SetCameraServoPosition( long milliseconds )
    {
        // Set to 90° --> pulsewdith = 1.5ms
        OCR1A = milliseconds * 2;
    }
}

void CCameraServo::Initialize()
{
    // Activate servo and set initial position
    tilt.Activate( CAMERAMOUNT_PIN );
    tilt.WriteMicroseconds( tiltTarget );

    // Mark camera servo as enabled
    NConfigManager::m_capabilityBitmask |= ( 1 << CAMERA_MOUNT_1_AXIS_CAPABLE );
}

void CCameraServo::Update( CCommand& command )
{
    // Check for messages
    if( NCommManager::m_isCommandAvailable )
    {
        // Handle messages
        if( command.Equals( "tilt" ) )
        {
            if( ( command.m_arguments[1] > CAMERA_SERVO_TARGET_MIN_US ) && ( command.m_arguments[1] < CAMERA_SERVO_TARGET_MAX_US ) )
            {
                commandedTilt = command.m_arguments[1];
                NDataManager::m_cameraMountData.CMTG = commandedTilt;
            }
        }

        if( command.Equals( "tiltInverted" ) )
        {
            if( ( command.m_arguments[1] == 1 )
            {
                // Set inverted
                m_isInverted = 1;

                // Report back
                Serial.print( F( "tiltInverted:1;" ) );
            }
            else if( command.m_arguments[1] == 0 )
            {
                // Set uninverted
                m_isInverted = 0;

                // Report back
                Serial.print( F( "tiltInverted:0;" ) );
            }
        }
    }

    // Adjust camera tilt smoothly towards the target
    if( commandedTilt != tiltTarget )
    {
        // Calculate the new tilt
        tiltTarget = SmoothlyAdjustCameraPosition( commandedTilt, tiltTarget );

        // Write to servo
        tilt.WriteMicroseconds( tiltTarget );

        NDataManager::m_cameraMountData.CMNT = tiltTarget;
    }
}

#endif
