#include "SysConfig.h"
#if(HAS_STD_LIGHTS)

// Includes
#include <Arduino.h>
#include "CDeadManSwitch.h"
#include <orutil.h>
#include "NModuleManager.h"
#include "NCommManager.h"

// Deadman Switch
#define DEADMAN_SWITCH_DELAY_TO_ARM_MS			180000

// File local variables and methods
namespace
{
	bool _deadmanSwitchEnabled	= false;
	bool blinkstate				= false;
	bool _deadmanSwitchArmed	= false;

	orutil::CTimer deadmanSwitchTimer;
	orutil::CTimer blinklightTimer;
}

void CDeadManSwitch::Initialize()
{
	deadmanSwitchTimer.Reset();
	blinklightTimer.Reset();
}

void CDeadManSwitch::Update( CCommand& command )
{
	// Check for messages
	if( NCommManager::m_isCommandAvailable )
	{
		if( command.Equals( "ping" ) )
		{
			deadmanSwitchTimer.Reset();

			if( _deadmanSwitchEnabled )
			{
				int32_t argsToSend[] = {0};
				command.PushCommand( "start", argsToSend );
				_deadmanSwitchEnabled = false;
			}

			Serial.print( F( "pong:" ) );
			Serial.print( command.m_arguments[0] );
			Serial.print( "," );
			Serial.print( millis() );
			Serial.print( ";" );
		}
		else if( command.Equals( "dms" ) )
		{
			if( command.m_arguments[0] == 0 )
			{
				_deadmanSwitchArmed = false;
			}
			else
			{
				_deadmanSwitchArmed = true;
			}
		}
	}

	//Auto arm the deadman switch after a reasonable boot time
	if( !_deadmanSwitchArmed && ( millis() > DEADMAN_SWITCH_DELAY_TO_ARM_MS ) )
	{
		_deadmanSwitchArmed = true;
	}

	if( ( deadmanSwitchTimer.HasElapsed( 2000 ) ) && _deadmanSwitchArmed && ( _deadmanSwitchEnabled == false ) )
	{
		int32_t argsToSend[] = {0}; //include number of parms as fist parm
		command.PushCommand( "deptloff", argsToSend );
		command.PushCommand( "headloff", argsToSend );
		command.PushCommand( "stop", argsToSend );
		_deadmanSwitchEnabled = true;
	}

	if( _deadmanSwitchEnabled && blinklightTimer.HasElapsed( 500 ) )
	{
		int32_t argsToSend[] = {1, 50};

		if( blinkstate )
		{
			argsToSend[1] = 0;
		}

		command.PushCommand( "ligt", argsToSend );
		blinkstate = !blinkstate;
	}
}
#endif
