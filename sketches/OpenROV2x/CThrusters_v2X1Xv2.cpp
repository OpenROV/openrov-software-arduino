#include "SysConfig.h"
#if( THRUSTER_CONFIGURATION == THRUSTER_CONFIG_v2X1Xv2 )

// Includes
#include "CThrusters.h"

#include "NVehicleManager.h"
#include "NDataManager.h"
#include "CMotor.h"
#include <orutil.h>
#include "CPin.h"

// Static variable initialization
const int CThrusters::kMotorCount = 5;

namespace
{
    CMotor port_forward_motor( PIN_PORT_FORWARD_MOTOR );
    CMotor port_aft_motor( PIN_PORT_AFT_MOTOR );
    CMotor vertical_motor( PIN_VERTICAL_MOTOR );
    CMotor starboard_forward_motor( PIN_STAR_FORWARD_MOTOR );
    CMotor starboard_aft_motor( STARBOARD_AFT_PIN );

    int new_pf	= MOTOR_TARGET_NEUTRAL_US;
    int new_pa	= MOTOR_TARGET_NEUTRAL_US;
    int new_sf	= MOTOR_TARGET_NEUTRAL_US;
    int new_sa	= MOTOR_TARGET_NEUTRAL_US;
    int new_v = MOTOR_TARGET_NEUTRAL_US;
    int new_st = 0; //strafe differential
    int pf = MOTOR_TARGET_NEUTRAL_US;
    int pa = MOTOR_TARGET_NEUTRAL_US;
    int sf = MOTOR_TARGET_NEUTRAL_US;
    int sa = MOTOR_TARGET_NEUTRAL_US;
    int v = MOTOR_TARGET_NEUTRAL_US;
    int st;

    float trg_throttle,trg_yaw,trg_lift,trg_strafe;
    int trg_motor_power;
    int maxHtransDelta;


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
    port_forward_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    port_forward_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    port_forward_motor.Activate();

    port_aft_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    port_aft_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    port_aft_motor.Activate();

    vertical_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    vertical_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    vertical_motor.Activate();

    starboard_forward_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    starboard_forward_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    starboard_forward_motor.Activate();

    starboard_aft_motor.m_negativeDeadzoneBuffer = NVehicleManager::m_deadZoneMin;
    starboard_aft_motor.m_positiveDeadzoneBuffer = NVehicleManager::m_deadZoneMax;
    starboard_aft_motor.Activate();

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
        port_forward_motor.m_positiveModifier = command.m_arguments[1] / 100;
        port_aft_motor.m_positiveModifier = port_forward_motor.m_positiveModifier;
        vertical_motor.m_positiveModifier = command.m_arguments[2] / 100;
        starboard_forward_motor.m_positiveModifier = command.m_arguments[3] / 100;
        starboard_aft_motor.m_positiveModifier = port_aft_motor.m_positiveModifier;
    }

    else if( command.Equals( "mtrmod2" ) )
    {
        port_forward_motor.m_negativeModifier = command.m_arguments[1] / 100;
        port_aft_motor.m_negativeModifier = port_forward_motor.m_negativeModifier;
        vertical_motor.m_negativeModifier = command.m_arguments[2] / 100;
        starboard_forward_motor.m_negativeModifier = command.m_arguments[3] / 100;
        starboard_aft_motor.m_negativeModifier = port_aft_motor.m_negativeModifier;
    }

    else if( command.Equals( "rmtrmod" ) )
    {
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_forward_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_aft_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_forward_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_aft_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_forward_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( port_aft_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_forward_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_aft_motor.m_negativeModifier );
        Serial.println( ";" );
    }

    //Legacy Control just the port motors, only used for manual callibration
    else if( command.Equals( "port" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            pf = command.m_arguments[1];
            pa = pf;

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    //Legacy Control, only used for manual callibration
    else if( command.Equals( "vertical" ) )
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

    //Legacy Control, only used for manual callibration
    else if( command.Equals( "starboard" ) )
    {
        //ignore corrupt data
        if( command.m_arguments[1] > 999 && command.m_arguments[1] < 2001 )
        {
            sf = command.m_arguments[1];
            sa = sf;

            if( command.m_arguments[2] == 1 )
            {
                bypasssmoothing = true;
            }
        }
    }

    //Since we have shared control surfaces (same motors that are used to thrust are used to yaw) we have to blend
    //the controls.  In this case, Throttle is the primary which means yaw is limited to whatever power still
    //remains in the motor range after processing the thrust.  In other words, if your at full thrust you will not
    //have any turn authority remaining.
    else if( command.Equals( "thro" ) || command.Equals( "yaw" ) || command.Equals("strafe") )
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

        else if (command.Equals("strafe")){
          if (command.m_arguments[1]>=-100 && command.m_arguments[1]<=100) {
            trg_strafe = command.m_arguments[1]/100.0;
          }
        }

        // The code below spreads the throttle spectrum over the possible range
        // of the motor. Not sure this belongs here or should be placed with
        // deadzon calculation in the motor code.
        if( trg_throttle >= 0 )
        {
            pf = 1500 + ( 500.0 / abs( port_forward_motor.m_positiveModifier ) ) * trg_throttle;
            sf = pf;
            pa = pf;
            sa = pf;
        }
        else
        {
            pf = 1500 + ( 500.0 / abs( port_forward_motor.m_negativeModifier ) ) * trg_throttle;
            sf = pf;
            pa = pf;
            sa = pf;
        }

        trg_motor_power = sf;

        int turn = trg_yaw * 250; //max range due to reverse range

        if( trg_throttle >= 0 )
        {
            int offset = ( abs( turn ) + trg_motor_power ) - 2000;

            if( offset < 0 )
            {
                offset = 0;
            }

            //This is the resulting of the blend of controls. In this thruster
            //configuration, the port thrusters push together to induce a yaw.
            pf = trg_motor_power + turn - offset;
            pa = pf;
            sf = trg_motor_power - turn - offset;
            sa = sf;
        }
        else
        {
            int offset = 1000 - ( trg_motor_power - abs( turn ) );

            if( offset < 0 )
            {
                offset = 0;
            }

            pf = trg_motor_power + turn + offset;
            pa = pf;
            sf = trg_motor_power - turn + offset;
            sa = sf;
        }

        //So in this configuration, the pf=sa and the sf=pa to work tother to strafe.  The pf will be opposit of sf.
        //strafe (side motion) is limited to whatever thrust is still available
        //from the thust and yaw.
        //maxHtransDelta = max range - remaining authority from throttle and yaw
        maxHtransDelta = abs((500.0/abs(port_forward_motor.m_negativeModifier))*(1.0-abs(trg_throttle)-abs(trg_yaw)));
        Serial.print("THRV2X1XV2.mhd:");Serial.print(maxHtransDelta);Serial.println(";");
        st = constrain( (500.0/abs(port_forward_motor.m_negativeModifier))*trg_strafe,-maxHtransDelta,maxHtransDelta);
        //st is the actual strafe translation we will employ
        Serial.print("THRV2X1XV2.st:");Serial.print(st);Serial.println(";");
        //Adjust the vertrans thrusters with the ideal translate value
        //if we go the wrong way... switch these.
        Serial.print("THRV2X1XV2.pf0:");Serial.print(pf);Serial.println(";");
        Serial.print("THRV2X1XV2.sf0:");Serial.print(sf);Serial.println(";");

        pf=pf+st;
        sa=sa+st;

        sf=sf-st;
        sa=sa-st;

        Serial.print("THRV2X1XV2.pf2:");Serial.print(pf);Serial.println(";");
        Serial.print("THRV2X1XV2.sf2:");Serial.print(sf);Serial.println(";");

    }

    else if( command.Equals( "lift" ) )
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
        if (pf!=new_pf || pa!=new_pa || v!=new_v || sf!=new_sf || sa!=new_sa)
        {
            new_pf = pf;
            new_pa = pa;
            new_v = v;
            new_sf = sf;
            new_sa = sa;

            // Check to see if any motors are non-neutral to signal system that at least one motor is running
            if( pf != MOTOR_TARGET_NEUTRAL_US || pa != MOTOR_TARGET_NEUTRAL_US ||v != MOTOR_TARGET_NEUTRAL_US || sf != MOTOR_TARGET_NEUTRAL_US || sa != MOTOR_TARGET_NEUTRAL_US )
            {
                NDataManager::m_thrusterData.MotorsActive = true;
            }
            else
            {
                NDataManager::m_thrusterData.MotorsActive = false;
            }

            Serial.print( F( "motors:" ) );
            Serial.print( port_forward_motor.SetMotorTarget( new_pf ) );
            Serial.print( ',' );
            Serial.print( port_aft_motor.SetMotorTarget( new_pa ) );
            Serial.print( ',' );
            Serial.print( vertical_motor.SetMotorTarget( new_v ) );
            Serial.print( ',' );
            Serial.print( starboard_forward_motor.SetMotorTarget( new_sf ) );
            Serial.print( ',' );
            Serial.print( starboard_aft_motor.SetMotorTarget( new_sa ) );
            Serial.println( ';' );
        }

    }

    NDataManager::m_navData.FTHR = map( ( new_pf + new_pa + new_sf + new_sa) / 4, 1000, 2000, -100, 100 );

    //The output from the motors is unique to the thruster configuration
    if( thrusterOutput.HasElapsed( 1000 ) )
    {
        Serial.print( F( "mtarg:" ) );
        Serial.print( pf );
        Serial.print( ',' );
        Serial.print( pa );
        Serial.print( ',' );
        Serial.print( v );
        Serial.print( ',' );
        Serial.print( sf );
        Serial.print( ',' );
        Serial.print( sa );
        Serial.println( ';' );
        NDataManager::m_thrusterData.MATC = port_forward_motor.IsActive() || port_aft_motor.IsActive() || vertical_motor.IsActive() || starboard_forward_motor.IsActive() || starboard_aft_motor.IsActive();
        Serial.print( F( "mtrmod:" ) );
        Serial.print( port_forward_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_aft_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_forward_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( starboard_aft_motor.m_positiveModifier );
        Serial.print( "," );
        Serial.print( port_forward_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( port_aft_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( vertical_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_forward_motor.m_negativeModifier );
        Serial.print( "," );
        Serial.print( starboard_aft_motor.m_negativeModifier );
        Serial.println( ";" );
    }
}

#endif
