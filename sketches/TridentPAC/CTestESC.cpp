#include "PinDefinitions.h"
#include <Arduino.h>

// Includes
#include "CTestESC.h"
#include "CTimer.h"
#include "NCommManager.h"
#include "NVehicleManager.h"
#include "Utility.h"
#include "fix16.h"
#include <math.h>

#define ESC_MESSAGE_SIZE            9

#define START_BYTE                  0x89
#define CMD_MOTOR_CONTROL_WRITE     0x08
#define CMD_PARAMETER_WRITE         0x06

#define PARAM_REVERSE               0x63
#define PARAM_CL_SETPOINT           0x0E
#define PARAM_OL_SETPOINT           0x0C
#define PARAM_AUTO_SWITCH           0x76


#define DEADZONE_MAGNITUDE          0.02f
#define TRANSITION_POINT            0.125f

#define OPEN_LOOP_MIN               0.0f
#define OPEN_LOOP_CEILING           21.447f
#define OPEN_LOOP_MAX               171.576f

#define CLOSE_LOOP_MIN              0.0f
#define CLOSE_LOOP_FLOOR            314.159f
#define CLOSE_LOOP_MAX              3500.0f


enum class ETransition
{
    NONE,
    CLOSED_TO_OPEN,
    OPEN_TO_CLOSED,
    OFF_TO_ON,
    ON_TO_OFF
};

enum class ERange
{
    OFF,
    OPEN,
    CLOSED
};

enum class EMotorDirection
{
    NORMAL = 0x00,
    REVERSE = 0x01
};

namespace
{
    CTimer m_controlTimer;

    uint8_t m_rxBuffer[ ESC_MESSAGE_SIZE ]      = {};
    int     m_bufferIndex                       = 0;

    uint8_t m_txBuffer[ ESC_MESSAGE_SIZE ]      = {};
    
    float m_throttle        = 0.0f;

    float m_clTarget        = 0.0f;
    fix16_t m_clTarget_f    = 0;
    float m_olTarget        = 0.0f;
    fix16_t m_olTarget_f    = 0;
    
    float portPos = 0.0f;
    float portNeg = 0.0f;
    float vertPos = 0.0f;
    float vertNeg = 0.0f;
    float starPos = 0.0f;
    float starNeg = 0.0f;
    
    EMotorDirection m_motorDir      = EMotorDirection::NORMAL;
    EMotorDirection m_motorDirPrev  = EMotorDirection::NORMAL;

    ERange m_range                  = ERange::OFF;
    ERange m_rangePrev              = ERange::OFF;

    ETransition m_transition        = ETransition::NONE;

    uint8_t Checksum( uint8_t* bufferIn )
    {
        uint8_t checksum = 0;
        int i;

        for (i = 0; i < 8; i++)
            checksum += *bufferIn++;

        return checksum;
    }

    void SendCommand( uint8_t startByte, uint8_t commandCode, uint8_t parameterCode, uint32_t data )
    {
        m_txBuffer[ 0 ] = startByte;
        m_txBuffer[ 1 ] = commandCode;
        m_txBuffer[ 2 ] = parameterCode;
        m_txBuffer[ 3 ] = 0;
        
        m_txBuffer[ 4 ] = ( ( data >> 24 ) & 0xFF );
        m_txBuffer[ 5 ] = ( ( data >> 16 ) & 0xFF );
        m_txBuffer[ 6 ] = ( ( data >> 8 )  & 0xFF );
        m_txBuffer[ 7 ] = ( data & 0xFF );

        m_txBuffer[ 8 ] = Checksum( (uint8_t*)&m_txBuffer );

        SerialMotor0.write( (char*)m_txBuffer, ESC_MESSAGE_SIZE );
        
        delay( 6 );
    }

    ERange DetectRange()
    {
        if( m_throttle < DEADZONE_MAGNITUDE )
        {
            return ERange::OFF;
        }
        else if( m_throttle >= DEADZONE_MAGNITUDE && m_throttle < TRANSITION_POINT )
        {
            return ERange::OPEN;
        }
        else
        {
            return ERange::CLOSED;
        }
    }

    bool SignChanged( float oldTarget, float newTarget )
    {
        return ( ( oldTarget * newTarget ) < 0.0f );
    }

    ETransition DetectRangeTransition()
    {
        if( m_rangePrev == m_range )
        {
            return ETransition::NONE;
        }
        else if( m_rangePrev == ERange::OFF )
        {
            return ETransition::OFF_TO_ON;
        }
        else if( m_range == ERange::OFF )
        {
            return ETransition::ON_TO_OFF;
        }
        else if( ( m_rangePrev == ERange::OPEN ) && ( m_range == ERange::CLOSED ) )
        {
            return ETransition::OPEN_TO_CLOSED;
        }
        else if( ( m_rangePrev == ERange::CLOSED ) && ( m_range == ERange::OPEN ) )
        {
            return ETransition::CLOSED_TO_OPEN;
        }
    }

    uint32_t CreateMotorControlCommand( bool isClosedLoop, bool isEnabled )
    {
        return ( 0 | ( 0 << 7 ) | ( isClosedLoop ? ( 1 << 1 ) : ( 0 << 1 ) ) | ( isEnabled ? ( 1 << 0 ) : ( 0 << 0 ) ) );
    }

    void DisableMotor()
    {
        // Send disable command to motor
        SendCommand( START_BYTE, CMD_MOTOR_CONTROL_WRITE, 0x00, CreateMotorControlCommand( false, false ) );
    }

    void EnableMotor()
    {
        if( m_range == ERange::OPEN )
        {
            // Open loop
            SendCommand( START_BYTE, CMD_MOTOR_CONTROL_WRITE, 0x00, CreateMotorControlCommand( false, true ) );
        }
        else if( m_range == ERange::CLOSED )
        {
            // Closed loop
            SendCommand( START_BYTE, CMD_MOTOR_CONTROL_WRITE, 0x00, CreateMotorControlCommand( true, true ) );
        }
        else
        {
            // Shouldn't get here! No need to enable motor if our target is in the deadzone
        }
    }

    void UpdateSetpoints()
    {
        SendCommand( START_BYTE, CMD_PARAMETER_WRITE, PARAM_OL_SETPOINT, m_olTarget_f );
        SendCommand( START_BYTE, CMD_PARAMETER_WRITE, PARAM_CL_SETPOINT, m_clTarget_f );
    }

    void SetMotorDirection()
    {
        if( m_motorDir == EMotorDirection::NORMAL )
        {
            SendCommand( START_BYTE, CMD_PARAMETER_WRITE, PARAM_REVERSE, 0x00 );
        }
        else
        {
            SendCommand( START_BYTE, CMD_PARAMETER_WRITE, PARAM_REVERSE, 0x01 );
        }
    }
    
    void EnableAutoSwitchMode( bool isEnabled )
    {
        SendCommand( START_BYTE, CMD_PARAMETER_WRITE, PARAM_AUTO_SWITCH, ( isEnabled ? 0x01 : 0x00 ) );
    }


    void HandleInput( float target )
    {
        // Get the direction based on the input
        m_motorDir = ( target < 0.0f ) ? EMotorDirection::REVERSE : EMotorDirection::NORMAL;

        // If it differs from the last update, trigger a direction change
        bool reversed       = ( m_motorDir != m_motorDirPrev );
        m_motorDirPrev      = m_motorDir;

        // Update throttle
        m_throttle          = fabs( target );

        // Get current range and transition, if one occurred
        m_range             = DetectRange();
        m_transition        = DetectRangeTransition();
        m_rangePrev         = m_range;

        // Calculate new setpoints
        if( m_range == ERange::OFF )
        {
            // Snap targets to 0.0f;
            m_clTarget = 0.0f;
            m_olTarget = 0.0f;
        }
        else
        {
            // Scale and floor/ceiling the targets appropriately
            m_clTarget = util::mapff( m_throttle, 0.0f, 1.0f, CLOSE_LOOP_MIN, CLOSE_LOOP_MAX );
            m_clTarget = ( m_clTarget < CLOSE_LOOP_FLOOR ) ? CLOSE_LOOP_FLOOR : m_clTarget;

            m_olTarget = util::mapff( m_throttle, 0.0f, 1.0f, OPEN_LOOP_MIN, OPEN_LOOP_MAX );
            m_olTarget = ( m_olTarget > OPEN_LOOP_CEILING ) ? OPEN_LOOP_CEILING : m_olTarget;
        }
        
        SerialDebug.print( "clTarget: " );
        SerialDebug.println( m_clTarget );
        
        SerialDebug.print( "olTarget: " );
        SerialDebug.println( m_olTarget );

        // Convert to fixed16_t
        m_olTarget_f = fix16_from_float( m_olTarget );
        m_clTarget_f = fix16_from_float( m_clTarget );

        // Handle transitions
        switch( m_transition )
        {
            case ETransition::NONE:
            {
                if( m_range != ERange::OFF )
                {
                    if( reversed )
                    {
                        // Stayed in same mode, switched direction
                        DisableMotor();
                        UpdateSetpoints();
                        SetMotorDirection();
                        EnableMotor();
                    }
                    else
                    {
                        // Stayed in the same mode and direction, just update setpoint
                        UpdateSetpoints();
                    }
                }
                else
                {
                    // Do nothing, motor is off
                }

                break;
            }

            case ETransition::OFF_TO_ON:
            {
                if( m_range == ERange::CLOSED )
                {
                    EnableAutoSwitchMode( true );
                }
                else
                {
                    EnableAutoSwitchMode( false );
                }
                
                SetMotorDirection();
                UpdateSetpoints();
                EnableMotor();
                
                SerialDebug.println( "Off->On" );
                
                break;
            }

            case ETransition::ON_TO_OFF:
            {
                DisableMotor();
                SerialDebug.println( "Motor off" );
                break;
            }

            case ETransition::OPEN_TO_CLOSED:
            {
                if( reversed )
                {
                    DisableMotor();
                    EnableAutoSwitchMode( true );
                    UpdateSetpoints();
                    SetMotorDirection();
                    EnableMotor();
                }
                else
                {
                    UpdateSetpoints();
                    EnableAutoSwitchMode( true );
                    EnableMotor();
                }
                
                SerialDebug.println( "Open->Close" );

                break;
            }

            case ETransition::CLOSED_TO_OPEN:
            {
                if( reversed )
                {
                    DisableMotor();
                    EnableAutoSwitchMode( false );
                    UpdateSetpoints();
                    SetMotorDirection();
                    EnableMotor();
                }
                else
                {
                    DisableMotor();
                    EnableAutoSwitchMode( false );
                    UpdateSetpoints();
                    EnableMotor();
                }
                
                SerialDebug.println( "Close->Open" );

                break;
            }

            default:
            {
                DisableMotor();
                EnableAutoSwitchMode( false );
                SerialDebug.println( "Unknown" );
                break;
            }
        }
    }

    void ProcessMessage()
    {
        fix16_t rxData;

        // Put data from uart_rx_buffer into word for processing (in case read)
        rxData = ( m_rxBuffer[4] << 24 );
        rxData |= ( m_rxBuffer[5] << 16 );
        rxData |= ( m_rxBuffer[6] << 8 );
        rxData |= m_rxBuffer[7];

        // Validate checksum
        if( Checksum( (uint8_t*)&m_rxBuffer) != m_rxBuffer[8] )
        {
            // Checksum error
        }
        else
        {
            uint8_t commandCode = m_rxBuffer[1] - 0x80;

            SerialDebug.print( "Message from ESC: " );
            SerialDebug.println( commandCode );

            // Switch on command code
            switch( commandCode )				
            {
                case 0:
                    break;
                default:
                    break;
            }
        }
    }

    void HandleComms()
    {
         // Read data from the ESC
        while( SerialMotor0.available() )
        {            
            char data = SerialMotor0.read();

            if( m_bufferIndex == 0 )                                    /* For the first byte received, the start byte must be 0xEE */
            {
                if( data != 0xEE )
                {
                    // Just skip bytes until we read the starting byte
                    continue;
                }
            }

            // Store byte and move to next index
            m_rxBuffer[ m_bufferIndex++ ] = data;

            // Check for complete message
            if( m_bufferIndex == ESC_MESSAGE_SIZE )
            {
                ProcessMessage();                                
                m_bufferIndex = 0;                                      
            }
        }
    }
}

void CTestESC::Initialize()
{
    NVehicleManager::m_capabilityBitmask |= ( 1 << LIGHTS_CAPABLE );
    NVehicleManager::m_capabilityBitmask |= ( 1 << DEPTH_CAPABLE );
    NVehicleManager::m_capabilityBitmask |= ( 1 << COMPASS_CAPABLE );
    NVehicleManager::m_capabilityBitmask |= ( 1 << ORIENTATION_CAPABLE );
    
    SerialDebug.println( "Starting esc" );
    m_controlTimer.Reset();
}

void CTestESC::Update( CCommand& command )
{
    //The output from the motors is unique to the thruster configuration
    if( m_controlTimer.HasElapsed( 1000 ) )
    {
        Serial.print( F( "mtarg:" ) );
        Serial.print( "1.0" );
        Serial.print( ',' );
        Serial.print( "1.0" );
        Serial.print( ',' );
        Serial.print( "1.0" );
        Serial.println( ';' );
        
        
        Serial.print( F( "mtrmod:" ) );
        Serial.print( portPos );
        Serial.print( "," );
        Serial.print( vertPos );
        Serial.print( "," );
        Serial.print( starPos );
        Serial.print( "," );
        Serial.print( portNeg );
        Serial.print( "," );
        Serial.print( vertNeg );
        Serial.print( "," );
        Serial.print( starNeg );
        Serial.println( ";" );
    }
    
    HandleComms();    

    // Update throttle
    if( NCommManager::m_isCommandAvailable )
    {
        if( command.Equals( "mtrmod1" ) )
        {
            portPos = command.m_arguments[1] / 100;
            vertPos = command.m_arguments[2] / 100;
            starPos = command.m_arguments[3] / 100;
        }

        if( command.Equals( "mtrmod2" ) )
        {
            portNeg = command.m_arguments[1] / 100;
            vertNeg = command.m_arguments[2] / 100;
            starNeg = command.m_arguments[3] / 100;
        }
        
        if( command.Equals( "rmtrmod" ) )
        {
            Serial.print( F( "mtrmod:" ) );
            Serial.print( portPos );
            Serial.print( "," );
            Serial.print( vertPos );
            Serial.print( "," );
            Serial.print( starPos );
            Serial.print( "," );
            Serial.print( portNeg );
            Serial.print( "," );
            Serial.print( vertNeg );
            Serial.print( "," );
            Serial.print( starNeg );
            Serial.println( ";" );
        }
        
        if( command.Equals( "thro" ) )
        {
            SerialDebug.println( "throttle" );
            
            if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 )
            {
                SerialDebug.print( "Gamepad input: " );
                SerialDebug.println( ( (float)command.m_arguments[1] / 100.0f )  );
                
                // For now, update as fast as we receive commands
                HandleInput( (float)command.m_arguments[1] / 100.0f );
            }
        }
    }    
}