#include "SysConfig.h"
#if(HAS_STD_CAMERAMOUNT)

// Includes
#include "CCameraMount.h"
#include <Servo.h>
#include "PinDefinitions.h"
#include "CPin.h"
#include "NVehicleManager.h"
#include "NDataManager.h"
#include "NCommManager.h"

// Conditional includes
#if MCUARCH == MCUARCH_AVR
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#define CAMERA_SERVO_TARGET_MIN_US				1000
#define CAMERA_SERVO_TARGET_MIDPOINT_US			1500
#define CAMERA_SERVO_TARGET_MAX_US				2000

// File local variables and methods
namespace
{
	
	#if MCUARCH == MCUARCH_AVR
    	// Use normal servo library if not on the CB25. We use a hacky workaround because of timing issues if on the CB25
		#if !( CONTROLLERBOARD == CONTROLLERBOARD_CB25 )
		    Servo tilt;
		#endif
	#else
		Servo tilt;
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

	#if MCUARCH == MCUARCH_AVR
	    void SetCameraServoPosition( long milliseconds )
	    {
	        // Set to 90° --> pulsewdith = 1.5ms
	        OCR1A = milliseconds * 2;
	    }
    #endif
}

void CCameraMount::Initialize()
{
	#if MCUARCH == MCUARCH_AVR
	    #if !( CONTROLLERBOARD == CONTROLLERBOARD_CB25 )
	    // Activate servo and set initial position
	    tilt.attach( PIN_CAMERA_MOUNT );
	    tilt.writeMicroseconds( tiltTarget );
	    #else
    
	    // Set up the pin for the camera servo
	    pinMode( PIN_CAMERA_MOUNT, OUTPUT );
	    TCCR1A = 0;
	    TCCR1B = 0;
	    TCCR1A |= ( 1 << COM1A1 ) | ( 1 << WGM11 );					// non-inverting mode for OC1A
	    TCCR1B |= ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );	// Mode 14, Prescaler 8
	
	    ICR1 = 40000; // 320000 / 8 = 40000
	
	    SetCameraServoPosition( CAMERA_SERVO_TARGET_MIDPOINT_US );
	    #endif
    #else
    	tilt.attach( PIN_CAMERA_MOUNT );
	    tilt.writeMicroseconds( tiltTarget );
    #endif

    // Mark camera servo as enabled
    NVehicleManager::m_capabilityBitmask |= ( 1 << CAMERA_MOUNT_1_AXIS_CAPABLE );
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

		#if MCUARCH == MCUARCH_AVR
	        // Write to servo
	        #if !( CONTROLLERBOARD == CONTROLLERBOARD_CB25 )
	        	tilt.writeMicroseconds( tiltTarget );
	        #else
	        	SetCameraServoPosition( tiltTarget );
	        #endif
        #else
        	tilt.writeMicroseconds( tiltTarget );
        #endif

        NDataManager::m_cameraMountData.CMNT = tiltTarget;
    }
}

#endif
