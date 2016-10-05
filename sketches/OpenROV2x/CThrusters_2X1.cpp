#include "SysConfig.h"
#if( THRUSTER_CONFIGURATION == THRUSTER_CONFIG_2X1 )

// Includes
#include "CThrusters.h"
#include "NVehicleManager.h"
#include "NDataManager.h"
#include "CMotor.h"
#include "CTimer.h"
#include "CPin.h"

#if(HAS_OROV_CONTROLLERBOARD_25)
    #include "CControllerBoard.h"
#endif


// Static variable initialization
const int CThrusters::kMotorCount = 3;

namespace
{
    CMotor port_motor( PIN_PORT_MOTOR );
    CMotor vertical_motor( PIN_VERTICAL_MOTOR );
    CMotor starboard_motor( PIN_STARBOARD_MOTOR );

    int new_p	= MOTOR_TARGET_NEUTRAL_US;
    int new_s	= MOTOR_TARGET_NEUTRAL_US;
    int new_v	= MOTOR_TARGET_NEUTRAL_US;
    int p		= MOTOR_TARGET_NEUTRAL_US;
    int v		= MOTOR_TARGET_NEUTRAL_US;
    int s		= MOTOR_TARGET_NEUTRAL_US;

    float trg_throttle, trg_yaw, trg_lift;
    int trg_motor_power;


    CTimer controltime;
    CTimer thrusterOutput;
    boolean bypasssmoothing;

#ifdef PIN_ENABLE_ESC
    bool canPowerESCs = true;
    CPin escpower( "escpower", PIN_ENABLE_ESC, CPin::kDigital, CPin::kOutput );
#else
    boolean canPowerESCs = false;
#endif
}

void CThrusters::Initialize()
{
    port_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    port_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    port_motor.Activate();

    vertical_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    vertical_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    vertical_motor.Activate();

    starboard_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    starboard_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    starboard_motor.Activate();

    thrusterOutput.Reset();
    controltime.Reset();

    bypasssmoothing = false;

    #ifdef PIN_ENABLE_ESC
    escpower.Reset();
    escpower.Write( 1 ); //Turn on the ESCs
    #endif
}

void CThrusters::Update( CCommand& command )
{
    if( command.Equals( "mtrmod1" ) )
    {
        port_motor.m_positiveModifier = command.m_arguments[1] / 100;
        vertical_motor.m_positiveModifier = command.m_arguments[2] / 100;
        starboard_motor.m_positiveModifier = command.m_arguments[3] / 100;
    }

    if( command.Equals( "mtrmod2" ) )
    {
        port_motor.m_negativeModifier = command.m_arguments[1] / 100;
        vertical_motor.m_negativeModifier = command.m_arguments[2] / 100;
        starboard_motor.m_negativeModifier = command.m_arguments[3] / 100;
    }

    if( command.Equals( "rmtrmod" ) )
    {
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_negativeModifier );
        Serial.println( ";" );
    }

    if( command.Equals( "go" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[2] > 999 && command.m_arguments[3] > 999 && command.m_arguments[1] < 2001 && command.m_arguments[2] < 2001 && command.m_arguments[3] < 2001 )
        {
            p = command.m_arguments[1];
            v = command.m_arguments[2];
            s = command.m_arguments[3];

            if( command.m_arguments[4] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    if( command.Equals( "port" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            p = command.m_arguments[1];

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    if( command.Equals( "vertical" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            v = command.m_arguments[1];

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    if( command.Equals( "starboard" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            s = command.m_arguments[1];

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    if( command.Equals( "thro" ) || command.Equals( "yaw" ) )
    {
        if( command.Equals( "thro" ) )
        {
            if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 )
            {
                trg_throttle = command.m_arguments[1] / 100.0;
            }
        }

        if( command.Equals( "yaw" ) )
        {
            //ignore corrupt data
            if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 ) //percent of max turn
            {
                trg_yaw = command.m_arguments[1] / 100.0;
            }
        }

        // The code below spreads the throttle spectrum over the possible range
        // of the motor. Not sure this belongs here or should be placed with
        // deadzon calculation in the motor code.
        if( trg_throttle >= 0 )
        {
            p = 1500 + ( 500.0 / abs( port_motor.m_positiveModifier ) ) * trg_throttle;
            s = p;
        }
        else
        {
            p = 1500 + ( 500.0 / abs( port_motor.m_negativeModifier ) ) * trg_throttle;
            s = p;
        }

        trg_motor_power = s;

        int turn = trg_yaw * 250; //max range due to reverse range

        if( trg_throttle >= 0 )
        {
            int offset = ( abs( turn ) + trg_motor_power ) - 2000;

            if( offset < 0 )
            {
                offset = 0;
            }

            p = trg_motor_power + turn - offset;
            s = trg_motor_power - turn - offset;
        }
        else
        {
            int offset = 1000 - ( trg_motor_power - abs( turn ) );

            if( offset < 0 )
            {
                offset = 0;
            }

            p = trg_motor_power + turn + offset;
            s = trg_motor_power - turn + offset;
        }

    }

    if( command.Equals( "lift" ) )
    {
        if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 )
        {
            trg_lift = command.m_arguments[1] / 100.0;
            v = 1500 + 500 * trg_lift;
        }
    }


    #ifdef PIN_ENABLE_ESC
    else if( command.Equals( "escp" ) )
    {
        escpower.Write( command.m_arguments[1] ); //Turn on the ESCs
        Serial.print( F( "log:escpower=" ) );
        Serial.print( command.m_arguments[1] );
        Serial.println( ';' );
    }

    #endif
    else if( command.Equals( "start" ) )
    {
        port_motor.Activate();
        vertical_motor.Activate();
        starboard_motor.Activate();
    }
    else if( command.Equals( "stop" ) )
    {
        p = MOTOR_TARGET_NEUTRAL_US;
        v = MOTOR_TARGET_NEUTRAL_US;
        s = MOTOR_TARGET_NEUTRAL_US;
        // Not sure why the reset does not re-attach the servo.
        //port_motor.stop();
        //vertical_motor.stop();
        //starboard_motor.stop();
    }

    #ifdef PIN_ENABLE_ESC
    else if( ( command.Equals( "mcal" ) ) && ( canPowerESCs ) )
    {
        Serial.println( F( "log:Motor Callibration Staring;" ) );
        //Experimental. Add calibration code here
        Serial.println( F( "log:Motor Callibration Complete;" ) );
    }

    #endif

    //to reduce AMP spikes, smooth large power adjustments out. This incirmentally adjusts the motors and servo
    //to their new positions in increments.  The incriment should eventually be adjustable from the cockpit so that
    //the pilot could have more aggressive response profiles for the ROV.
    if( controltime.HasElapsed( 50 ) )
    {
        if( p != new_p || v != new_v || s != new_s )
        {
            new_p = p;
            new_v = v;
            new_s = s;

            // Check to see if any motors are non-neutral to signal system that at least one motor is running
            if( p != MOTOR_TARGET_NEUTRAL_US || v != MOTOR_TARGET_NEUTRAL_US || s != MOTOR_TARGET_NEUTRAL_US )
            {
                NDataManager::m_thrusterData.MotorsActive = true;
            }
            else
            {
                NDataManager::m_thrusterData.MotorsActive = false;
            }

            Serial.print( F( "motors:" ) );
            Serial.print( port_motor.SetMotorTarget( new_p ) );
            Serial.print( ',' );
            Serial.print( vertical_motor.SetMotorTarget( new_v ) );
            Serial.print( ',' );
            Serial.print( starboard_motor.SetMotorTarget( new_s ) );
            Serial.println( ';' );
        }

    }

    NDataManager::m_navData.FTHR = map( ( new_p + new_s ) / 2, 1000, 2000, -100, 100 );

    //The output from the motors is unique to the thruster configuration
    if( thrusterOutput.HasElapsed( 1000 ) )
    {
        Serial.print( F( "mtarg:" ) );
        Serial.print( p );
        Serial.print( ',' );
        Serial.print( v );
        Serial.print( ',' );
        Serial.print( s );
        Serial.println( ';' );
        NDataManager::m_thrusterData.MATC = port_motor.IsActive() || port_motor.IsActive() || port_motor.IsActive();
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_negativeModifier );
        Serial.println( ";" );
    }
}

#endif