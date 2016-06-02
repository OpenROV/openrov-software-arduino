#include "AConfig.h"
#if(HAS_STD_CAMERAMOUNT)

// Includes
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "CCameraMount.h"
#include "CServo.h"
#include "CPin.h"
#include "NConfigManager.h"
#include "NDataManager.h"
#include "NModuleManager.h"
#include "NCommManager.h"

// One of these will define the pin number for the camera mount
#if(HAS_STD_CAPE)
    #include "CCape.h"
#endif

#if(HAS_OROV_CONTROLLERBOARD_25)
    #include "CControllerBoard.h"
#endif

// Defines
#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

// File local variables and methods
namespace
{
    // Use normal servo library if the camera servo is not on pin 11, otherwise use AVR IO methods
#if( CAMERAMOUNT_PIN != 11 )
    CServo tilt;
#endif

    int commandedTilt	= CAMERA_SERVO_TARGET_MIDPOINT_US;
    int tiltTarget		= CAMERA_SERVO_TARGET_MIDPOINT_US;

    const int tiltRate	= 1;

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

void CCameraMount::Initialize()
{
    #if(CAMERAMOUNT_PIN != 11)
    // Activate servo and set initial position
    tilt.Activate( CAMERAMOUNT_PIN );
    tilt.WriteMicroseconds( tiltTarget );
    #else
    // Set up the pin for the camera servo
    pinMode( CAMERAMOUNT_PIN, OUTPUT );
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1A |= ( 1 << COM1A1 ) | ( 1 << WGM11 );					// non-inverting mode for OC1A
    TCCR1B |= ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );	// Mode 14, Prescaler 8

    ICR1 = 40000; // 320000 / 8 = 40000

    SetCameraServoPosition( CAMERA_SERVO_TARGET_MIDPOINT_US );
    #endif

    // Mark camera servo as enabled
    NConfigManager::m_capabilityBitmask |= ( 1 << CAMERA_MOUNT_1_AXIS_CAPABLE );
}

void CCameraMount::Update( CCommand& command )
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
    }

    // Adjust camera tilt smoothly towards the target
    if( commandedTilt != tiltTarget )
    {
        // Calculate the new tilt
        tiltTarget = SmoothlyAdjustCameraPosition( commandedTilt, tiltTarget );

        // Write to servo
        #if( CAMERAMOUNT_PIN != 11 )
        tilt.WriteMicroseconds( tiltTarget );
        #else
        SetCameraServoPosition( tiltTarget );
        #endif

        NDataManager::m_cameraMountData.CMNT = tiltTarget;
    }
}

#endif
