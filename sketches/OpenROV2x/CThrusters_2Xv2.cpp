#include "SysConfig.h"
#if( THRUSTER_CONFIGURATION == THRUSTER_CONFIG_2Xv2 )

// Includes
#include "CThrusters.h"
#include "NVehicleManager.h"
#include "NDataManager.h"
#include "CMotor.h"
#include <orutil.h>
#include "CPin.h"

// Static variable initialization
const int CThrusters::kMotorCount = 4;

namespace
{
    CMotor port_motor( PIN_PORT_MOTOR );
    CMotor port_vertical_motor( PIN_PORT_VERTICAL_MOTOR );
    CMotor starboard_motor( PIN_STARBOARD_MOTOR );
    CMotor starboard_vertical_motor( PIN_STAR_VERTICAL_MOTOR );

    int new_p	= MOTOR_TARGET_NEUTRAL_US;
    int new_s	= MOTOR_TARGET_NEUTRAL_US;
    int new_vp = MOTOR_TARGET_NEUTRAL_US;
    int new_vs = MOTOR_TARGET_NEUTRAL_US;
    int new_st = 0; //strafe differential
    int p = MOTOR_TARGET_NEUTRAL_US;
    int vp = MOTOR_TARGET_NEUTRAL_US;
    int vs = MOTOR_TARGET_NEUTRAL_US;
    int vp2 = MOTOR_TARGET_NEUTRAL_US;
    int vs2 = MOTOR_TARGET_NEUTRAL_US;
    int s = MOTOR_TARGET_NEUTRAL_US;
    int st;

    float trg_throttle,trg_yaw,trg_lift,trg_strafe;
    int trg_motor_power;
    int maxVtransDelta;


    orutil::CTimer controltime;
    orutil::CTimer thrusterOutput;
    bool bypasssmoothing;

    bool isGreeting = false;
    const int greetDelay_ms = 100;

#ifdef PIN_ENABLE_ESC
    bool canPowerESCs = true;
    CPin escpower( PIN_ENABLE_ESC, CPin::kDigital, CPin::kOutput );
#else
    bool canPowerESCs = false;
#endif
}

void CThrusters::Initialize()
{
    port_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    port_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    port_motor.Activate();

    port_vertical_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    port_vertical_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    port_vertical_motor.Activate();

    starboard_vertical_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    starboard_vertical_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    starboard_vertical_motor.Activate();

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
        port_vertical_motor.m_positiveModifier = command.m_arguments[2] / 100;
        starboard_vertical_motor.m_positiveModifier = command.m_arguments[2] / 100;
        starboard_motor.m_positiveModifier = command.m_arguments[3] / 100;
    }

    else if( command.Equals( "mtrmod2" ) )
    {
        port_motor.m_negativeModifier = command.m_arguments[1] / 100;
        port_vertical_motor.m_negativeModifier = command.m_arguments[2] / 100;
        starboard_vertical_motor.m_negativeModifier = command.m_arguments[2] / 100;
        starboard_motor.m_negativeModifier = command.m_arguments[3] / 100;
    }

    else if( command.Equals( "rmtrmod" ) )
    {
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( port_vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_negativeModifier );
        Serial.println( ";" );
    }

    else if( command.Equals( "port" ) )
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

    else if( command.Equals( "vertical" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            vp = command.m_arguments[1];
            vs = vp;

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    else if( command.Equals( "starboard" ) )
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

    else if( command.Equals( "thro" ) || command.Equals( "yaw" ) )
    {
        if( command.Equals( "thro" ) )
        {
            if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 )
            {
                trg_throttle = command.m_arguments[1] / 100.0;
            }
        }

        else if( command.Equals( "yaw" ) )
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

    else if( command.Equals( "lift" ) )
    {
        if( command.m_arguments[1] >= -100 && command.m_arguments[1] <= 100 )
        {
            trg_lift = command.m_arguments[1] / 100.0;
            vp = 1500 + 500 * trg_lift;
            vs = vp;
        }
    }
    else if (command.Equals("lift") || command.Equals("strafe")  ){
      if (command.Equals("lift")){
        if (command.m_arguments[1]>=-100 && command.m_arguments[1]<=100) {
          trg_lift = command.m_arguments[1]/100.0;

          //the vertical component of the thrust factor is
          //vThrust = sin(65)*thrust

          //This spreads the percentage power over the fully available
          //range of the thruster.  Since the current thrusters
          //are less powerfull in the reverse direction and both motors
          //should provide the same thrust, the maximum thurst is
          //governed by the negative modifier of the motor.

          //assume that port and starboard vertical thrusters
          //have the same modifiers for the moment.
          vp = 1500 + 500 * trg_lift;
          vs = 1500 + 500 * trg_lift;
          vp2 = vp;
          vs2 = vs;
          Serial.print("THR2XV2.vp1:");Serial.print(vp);Serial.println(";");
          Serial.print("THR2XV2.vs1:");Serial.print(vs);Serial.println(";");
        }
      }

      else if (command.Equals("strafe")){
        if (command.m_arguments[1]>=-100 && command.m_arguments[1]<=100) {
          trg_strafe = command.m_arguments[1]/100.0;
          //strafe (side motion) is limited to whatever thrust is still available
          //from the vertical thruster range.  If vertical is full power,
          //the strafe will be zero.
          maxVtransDelta = abs((500.0/abs(port_vertical_motor.m_negativeModifier))*(1.0-abs(trg_lift)));
          Serial.print("THR2XV2.mvd:");Serial.print(maxVtransDelta);Serial.println(";");
          st = constrain( (500.0/abs(port_vertical_motor.m_negativeModifier))*trg_strafe,-maxVtransDelta,maxVtransDelta);
          Serial.print("THR2XV2.st:");Serial.print(st);Serial.println(";");
          //Adjust the vertrans thrusters with the ideal translate value
          //if we go the wrong way... switch these.
          Serial.print("THR2XV2.vp0:");Serial.print(vp);Serial.println(";");

          vp2=vp+st;
          vs2=vs-st;
          Serial.print("THR2XV2.vp2:");Serial.print(vp2);Serial.println(";");
          Serial.print("THR2XV2.vs2:");Serial.print(vs2);Serial.println(";");
        }
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

    else if( command.Equals( "wake" ) )
    {
        // Set greeting state to true and reset timer
        isGreeting = true;
        controltime.Reset();

        // Turn off ESCs
        #ifdef PIN_ENABLE_ESC
            escpower.Write( 0 );
        #else
            port_motor.Deactivate();
            vertical_motor.Deactivate();
            port_motor.Deactivate();
        #endif
    }

    if( isGreeting )
    {
        if( controltime.HasElapsed( greetDelay_ms ) )
        {
            // Turn on ESCs
            #ifdef PIN_ENABLE_ESC
                escpower.Write( 1 );
            #else
                port_motor.Activate();
                vertical_motor.Activate();
                port_motor.Activate();
            #endif

            isGreeting = false;
        }
        else
        {
            return;
        }
    }

    //to reduce AMP spikes, smooth large power adjustments out. This incirmentally adjusts the motors and servo
    //to their new positions in increments.  The incriment should eventually be adjustable from the cockpit so that
    //the pilot could have more aggressive response profiles for the ROV.
    if( controltime.HasElapsed( 50 ) )
    {
        if (p!=new_p || vp2!=new_vp || vs2!=new_vs || s!=new_s)
        {
            new_p = p;
            new_vp = vp2;
            new_vs = vs2;
            new_s = s;

            // Check to see if any motors are non-neutral to signal system that at least one motor is running
            if( p != MOTOR_TARGET_NEUTRAL_US || vp != MOTOR_TARGET_NEUTRAL_US || vs != MOTOR_TARGET_NEUTRAL_US || s != MOTOR_TARGET_NEUTRAL_US )
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
            Serial.print( port_vertical_motor.SetMotorTarget( new_vp ) );
            Serial.print( ',' );
            Serial.print( starboard_vertical_motor.SetMotorTarget( new_vs ) );
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
        Serial.print( vp );
        Serial.print( ',' );
        Serial.print( vs );
        Serial.print( ',' );
        Serial.print( s );
        Serial.println( ';' );
        NDataManager::m_thrusterData.MATC = port_motor.IsActive() || port_motor.IsActive() || port_motor.IsActive();
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( port_vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_motor.m_negativeModifier );
        Serial.println( ";" );
    }
}

#endif
