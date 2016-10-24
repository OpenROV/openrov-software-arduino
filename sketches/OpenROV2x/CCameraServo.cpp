#include "SysConfig.h"
#if(HAS_CAMERASERVO)

// Includes
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "CCameraServo.h"
#include <Servo.h>
#include "CPin.h"
#include "NDataManager.h"
#include "NModuleManager.h"
#include "NCommManager.h"
#include <orutil.h>

#include "CControllerBoard.h"

#include <math.h>

// Defines
#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

// File local variables and methods
namespace
{
    constexpr float kZeroPosMicrosecs       = 1487.0f;
    constexpr float kMicrosecPerDegree      = 9.523809f;
    constexpr float kDegPerMicrosec         = ( 1 / kMicrosecPerDegree );

    // Helper functions specifically for the HITEC servo
    constexpr uint32_t DegreesToMicroseconds( float degreesIn, bool isInverted = false )
    {
        return static_cast<uint32_t>( ( kMicrosecPerDegree * ( isInverted ? -degreesIn : degreesIn ) ) + kZeroPosMicrosecs );
    }

    constexpr float MicrosecondsToDegrees( uint32_t microsecondsIn, bool isInverted = false )
    {
        return ( isInverted ?   -( ( static_cast<float>( microsecondsIn ) - kZeroPosMicrosecs ) * kDegPerMicrosec )
                                :( ( static_cast<float>( microsecondsIn ) - kZeroPosMicrosecs ) * kDegPerMicrosec ) );
    }

    constexpr float kNeutralPosition_deg    = 0.0f;
    constexpr uint32_t kNeutralPosition_us  = DegreesToMicroseconds( 0.0f );

    constexpr float kDefaultSpeed           = 50.0f;    // Degrees per sec

    // Attributes
    orutil::CTimer m_controlTimer;
    orutil::CTimer m_telemetryTimer;

    float m_targetPos_deg       = kNeutralPosition_deg;
    float m_currentPos_deg      = kNeutralPosition_deg;
    uint32_t m_targetPos_us     = kNeutralPosition_us;
    uint32_t m_currentPos_us    = kNeutralPosition_us;
    float m_fCurrentPos_us      = kZeroPosMicrosecs;

    uint32_t m_tDelta           = 0;
    uint32_t m_tLast            = 0;

    // Settings
    float m_speed_deg_per_s     = kDefaultSpeed;
    int m_isInverted            = 0;                // 0 - Not inverted, 1 - Inverted

    // Derived from settings
    float m_speed_us_per_ms     = ( kDefaultSpeed * 0.001f ) * kMicrosecPerDegree;

    // Float<->Int conversion helpers
    constexpr int32_t Encode( float valueIn )
    {
        return static_cast<int32_t>( valueIn * 1000.0f );
    }

    constexpr float Decode( int32_t valueIn )
    {
        return ( static_cast<float>( valueIn ) * 0.001f );
    }

    void SetServoPosition( uint32_t microsecondsIn )
    {
        // Set to 90° --> pulsewdith = 1.5ms
        OCR1A = microsecondsIn * 2;
    }
}

void CCameraServo::Initialize()
{
    // Set up the pin for the camera servo
    pinMode( PIN_CAMERA_MOUNT, OUTPUT );

    // Set up the timers driving the PWM for the servo (AVR specific)
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1A |= ( 1 << COM1A1 ) | ( 1 << WGM11 );					// non-inverting mode for OC1A
    TCCR1B |= ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );	// Mode 14, Prescaler 8

    ICR1 = 40000; // 320000 / 8 = 40000

    // Set initial position
    SetServoPosition( kNeutralPosition_us );

    // Reset timers
    m_controlTimer.Reset();
    m_telemetryTimer.Reset();
}

void CCameraServo::Update( CCommand& command )
{
    // Check for messages
    if( NCommManager::m_isCommandAvailable )
    {
        // Handle messages
        if( command.Equals( "camServ_tpos" ) )
        {
            // TODO: Ideally this unit would have the ability to autonomously set its own target and ack receipt with a separate mechanism
            // Acknowledge target position
            Serial.print( F( "camServ_tpos:" ) );
            Serial.print( static_cast<int32_t>( command.m_arguments[1] ) );
            Serial.println( ';' );
            
            // Update the target position
            m_targetPos_deg = Decode( static_cast<int32_t>( command.m_arguments[1] ) );

            // Update the target microseconds
            m_targetPos_us = DegreesToMicroseconds( m_targetPos_deg, m_isInverted );
        }
        else if( command.Equals( "camServ_spd" ) )
        {
            // Acknowledge receipt of command
            Serial.print( F( "camServ_spd:" ) );
            Serial.print( static_cast<int32_t>( command.m_arguments[1] ) );
            Serial.println( ';' );

            // Decode the requested speed and update the setting
            m_speed_deg_per_s   = Decode( static_cast<int32_t>( command.m_arguments[1] ) );
            m_speed_us_per_ms   = ( m_speed_deg_per_s * 0.001f ) * kMicrosecPerDegree;
        }
        else if( command.Equals( "camServ_inv" ) )
        {
            if( command.m_arguments[1] == 1 )
            {
                // Set inverted
                m_isInverted = 1;

                // Report back
                Serial.print( F( "camServ_inv:1;" ) );
            }
            else if( command.m_arguments[1] == 0 )
            {
                // Set uninverted
                m_isInverted = 0;

                // Report back
                Serial.print( F( "camServ_inv:0;" ) );
            }

            // Adjust target_us based on inversion
            m_targetPos_us = DegreesToMicroseconds( m_targetPos_deg, m_isInverted );
        }
    }

    // Run servo adjustment at 200Hz
    if( m_controlTimer.HasElapsed( 5 ) )
    {
        // Get time elapsed since last position update
        m_tDelta = millis() - m_tLast;

        // Update position if not at desired location
        if( m_currentPos_us != m_targetPos_us )
        {
            float error = static_cast<float>( m_targetPos_us ) - m_fCurrentPos_us;

            // Check to see if the error/dT is smaller than the speed limit
            if( abs( error / static_cast<float>( m_tDelta ) ) < m_speed_us_per_ms )
            {
                // Move directly to the target
                // NOTE: Cannot use the cast method like below, since the floating point
                // representation of the target pos might be comparatively less than the integer value.
                // I.e. target = 32, static_cast<float>( 32 ) -> 31.99999, static_cast<uint32_t>( 31.99999 ) -> 31
                // This could lead to the current position never reaching the target
                m_currentPos_us = m_targetPos_us;

                // Update the floating point representation as well, for use in future target updates
                m_fCurrentPos_us = static_cast<float>( m_targetPos_us );
            }
            else
            {
                // Move by the delta towards the target
                m_fCurrentPos_us += ( ( ( error < 0 ) ? -m_speed_us_per_ms : m_speed_us_per_ms ) * static_cast<float>( m_tDelta ) );

                // Cast the floating point servo command to an integer
                m_currentPos_us = static_cast<uint32_t>( m_fCurrentPos_us );
            }
            
            // Set the servo to this target
            SetServoPosition( m_currentPos_us );

            // Update the position value in degrees
            m_currentPos_deg = MicrosecondsToDegrees( m_currentPos_us );
        }

        m_tLast = millis();
    }

    // Emit position telemetry at 3Hz
    if( m_telemetryTimer.HasElapsed( 333 ) )
    {
        Serial.print( F( "camServ_pos:" ) );
        Serial.print( Encode( m_currentPos_deg ) );
        Serial.println( ';' );
    }
}

#endif
