#include "SysConfig.h"
#if(HAS_STD_AUTOPILOT)

#include "CAutopilot.h"
#include <orutil.h>
#include "NDataManager.h"
#include "NCommManager.h"

namespace
{
	orutil::CTimer pilotTimer;

	bool _headingHoldEnabled = false;
	int  _headingHoldTarget = 0;
	int hdg = 0;
	int hdg_Error;
	int raw_Left, raw_Right;
	float heading_loop_Gain = 1.0;
	float depth_hold_loop_gain = 0.6;
	long hdg_Error_Integral = 0;
	int tgt_Hdg = 0;
	bool _depthHoldEnabled = false;
	int _depthHoldTarget = 0;
	int depth = 0;
	int depth_Error = 0;
	int raw_lift = 0;
	int lift = 0;
	int target_depth;
	int raw_yaw, yaw;

	int depth_deadband = 4; // +/- cm
	int heading_deadband = 4;  // +/i degrees

}

void CAutopilot::Initialize()
{
	pilotTimer.Reset();

	Serial.println( F( "log:pilot setup complete;" ) );
}



void CAutopilot::Update( CCommand& command )
{
	//intended to respond to fly by wire commands: MaintainHeading(); TurnTo(compassheading); DiveTo(depth);

	if( NCommManager::m_isCommandAvailable )
	{
		
		if( command.Equals( "headloff" ) || command.Equals( "holdHeading_off" )  )
		{
			_headingHoldEnabled = false;
			raw_Left = 0;
			raw_Right = 0;
			hdg_Error_Integral = 0;  // Reset error integrator
			tgt_Hdg = -500;  // -500 = system not in hdg hold
	
			int32_t m_argumentsToSend[] = {1, 00}; //include number of parms as fist parm
			command.PushCommand( "yaw", m_argumentsToSend );
			Serial.println( F( "log:heading_hold_disabled;" ) );
			Serial.print( F( "targetHeading:" ) );
			Serial.print( F("DISABLED") );
			Serial.println( ';' );
	
		}
	
		if( command.Equals( "headlon" ) || command.Equals( "holdHeading_on" ) )
		{
			_headingHoldEnabled = true;
	
			if( command.m_arguments[0] == 0 )
			{
				_headingHoldTarget = NDataManager::m_navData.YAW;
			}
			else
			{
				_headingHoldTarget = command.m_arguments[1];
			}
	
			tgt_Hdg = _headingHoldTarget;
			Serial.print( F( "log:heading_hold_enabled on=" ) );
			Serial.print( tgt_Hdg );
			Serial.println( ';' );
			Serial.print( F( "targetHeading:" ) );
			Serial.print( tgt_Hdg );
			Serial.println( ';' );
		}
	
		//Backwards compatibility for a release or two (2.5.1 release)
		if( command.Equals( "holdHeading_toggle" ) )
		{
			if( _headingHoldEnabled )
			{
				int32_t m_argumentsToSend[] = {0}; //include number of parms as fist parm
				command.PushCommand( "headloff", m_argumentsToSend );
			}
			else
			{
				if( command.m_arguments[0] == 0 )
				{
					int32_t m_argumentsToSend[] = {0}; //include number of parms as fist parm
					command.PushCommand( "headlon", m_argumentsToSend );
				}
				else
				{
					int32_t m_argumentsToSend[] = {1, command.m_arguments[1]}; //include number of parms as fist parm
					command.PushCommand( "headlon", m_argumentsToSend );
				}
	
			}
		}
	
		if( command.Equals( "deptloff" ) || command.Equals( "holdDepth_off" ) )
		{
			_depthHoldEnabled = false;
			raw_lift = 0;
			target_depth = 0;
	
			int32_t m_argumentsToSend[] = {1, 0}; //include number of parms as fist parm
			command.PushCommand( "lift", m_argumentsToSend );
			Serial.println( F( "log:depth_hold_disabled;" ) );
			Serial.print( F( "targetDepth:" ) );
			Serial.print( F("DISABLED") );
			Serial.println( ';' );
	
		}
	
		if( command.Equals( "deptlon" ) || command.Equals( "holdDepth_on" ) )
		{
			_depthHoldEnabled = true;
	
			if( command.m_arguments[0] == 0 )
			{
				_depthHoldTarget = NDataManager::m_navData.DEEP * 100; //casting to cm
			}
			else
			{
				_depthHoldTarget = command.m_arguments[1];
			}
	
			target_depth = _depthHoldTarget;
			Serial.print( F( "log:depth_hold_enabled on=" ) );
			Serial.print( target_depth );
			Serial.println( ';' );
			Serial.print( F( "targetDepth:" ) );
			Serial.print( target_depth );
			Serial.println( ';' );
		}
	
	
		if( command.Equals( "holdDepth_toggle" ) )
		{
			if( _depthHoldEnabled )
			{
				int32_t m_argumentsToSend[] = {0}; //include number of parms as fist parm
				command.PushCommand( "deptloff", m_argumentsToSend );
			}
			else
			{
				if( command.m_arguments[0] == 0 )
				{
					int32_t m_argumentsToSend[] = {0}; //include number of parms as fist parm
					command.PushCommand( "deptlon", m_argumentsToSend );
				}
				else
				{
					int32_t m_argumentsToSend[] = {1, command.m_arguments[1]}; //include number of parms as fist parm
					command.PushCommand( "deptlon", m_argumentsToSend );
				}
			}
		}
	}


	if( pilotTimer.HasElapsed( 50 ) )
	{

		// Autopilot Test #3 6 Jan 2014
		// Hold vehicle at arbitrary heading
		// Integer math; proportional control plus basic integrator
		// No hysteresis around 180 degree error

		// Check whether hold mode is on

		if( _depthHoldEnabled )
		{
			depth = NDataManager::m_navData.DEEP * 100;
			depth_Error = target_depth - depth; //positive error = positive lift = go deeper.

			raw_lift = ( float )depth_Error * depth_hold_loop_gain;
			lift = constrain( raw_lift, -100, 100 );

			Serial.println( F( "log:dhold pushing command;" ) );
			Serial.print( F( "dp_er:" ) );
			Serial.print( depth_Error );
			Serial.println( ';' );

			if( abs( depth_Error ) > depth_deadband )
			{
				int32_t m_argumentsToSend[] = {1, lift}; //include number of parms as fist parm
				command.PushCommand( "lift", m_argumentsToSend );
			}
			else
			{
				int32_t m_argumentsToSend[] = {1, 0}; //include number of parms as fist parm
				command.PushCommand( "lift", m_argumentsToSend );
			}

		}

		if( _headingHoldEnabled )
		{

			// Code for hold mode here
			hdg = NDataManager::m_navData.YAW;

			// Calculate heading error

			hdg_Error = hdg - tgt_Hdg;

			if( hdg_Error > 180 )
			{
				hdg_Error = hdg_Error - 360;
			}

			if( hdg_Error < -179 )
			{
				hdg_Error = hdg_Error + 360;
			}

			// Run error accumulator (integrator)
			hdg_Error_Integral = hdg_Error_Integral + hdg_Error;

			// Calculator motor outputs
			raw_yaw = -1 * hdg_Error * heading_loop_Gain;

			// raw_Left = raw_Left - (hdg_Error_Integral / integral_Divisor);
			// raw_Right = raw_Right + (hdg_Error_Integral / integral_Divisor);

			// Constrain and output to motors

			yaw = constrain( raw_yaw, -50, 50 );
			Serial.println( F( "log:hold pushing command;" ) );
			Serial.print( F( "p_er:" ) );
			Serial.print( hdg_Error );
			Serial.println( ';' );

			if( abs( hdg_Error ) > heading_deadband )
			{
				//start the motor with least power
				if( hdg_Error > 0 )
				{
					hdg_Error -= heading_deadband;
				}
				else
				{
					hdg_Error += heading_deadband;
				}

				int32_t m_argumentsToSend[] = {1, yaw}; //include number of parms as fist parm
				command.PushCommand( "yaw", m_argumentsToSend );
			}
			else
			{
				int32_t m_argumentsToSend[] = {1, 0}; //include number of parms as fist parm
				command.PushCommand( "yaw", m_argumentsToSend );
			}
		}


	}
}
#endif
