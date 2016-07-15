#include "AConfig.h"
#if( HAS_EXP_AUTOPILOT )

// Includes
#include "CAutopilot.h"
#include "CTimer.h"
#include "NDataManager.h"
#include "NCommManager.h"

#include "CPIDController.h"
#include "Utility.h"

// File local variables and methods
namespace
{
	CTimer pilotTimer;

	// These should be replaced with two CPIDController instances for yaw and depth
	CPIDControllerAngular	m_yawController( 0.01f, 0.0f, 0.0f, -1.0f, 1.0f, PID_CONTROLLER_DIRECTION_DIRECT, 10 );
	CPIDControllerLinear	m_depthController( 0.01f, 0.0f, 0.0f, -1.0f, 1.0f, PID_CONTROLLER_DIRECTION_DIRECT, 10 );

	void HandleCommands()
	{
		if( !NCommManager::m_isCommandAvailable )
		{
			// No commands to process
			return;
		}

		// Disable heading controller
		if( NCommManager::m_currentCommand.Equals( "headloff" ) )
		{
			// Deactivate PID controller
			m_yawController.Deactivate();

			// Send command to drive yaw at zero capacity (to turn off whatever was last sent by the controller)
			int m_argumentsToSend[] = { 1, 0 };
			NCommManager::m_currentCommand.PushCommand( "yaw", m_argumentsToSend );

			Serial.println( F( "log:heading_hold_disabled;" ) );

			Serial.print( F( "targetHeading:" ) );
			Serial.print( DISABLED );
			Serial.println( ';' );

			return;
		}

		// Enable heading controller
		if( NCommManager::m_currentCommand.Equals( "headlon" ) )
		{
			if( NCommManager::m_currentCommand.m_arguments[0] == 0 )
			{
				// Set the new target to our current heading
				NDataManager::m_controllerData.yawSetpoint = NDataManager::m_controllerData.yaw;
			}
			else
			{
				// Set new setpoint to the specified target value
				NDataManager::m_controllerData.yawSetpoint = NCommManager::m_currentCommand.m_arguments[1];
			}

			// Activate PID controller
			m_yawController.Activate();

			Serial.print( F( "log:heading_hold_enabled on=" ) );
			Serial.print( *m_yawController.m_pSetpoint );
			Serial.println( ';' );

			Serial.print( F( "targetHeading:" ) );
			Serial.print( *m_yawController.m_pSetpoint );
			Serial.println( ';' );

			return;
		}

		// Disable depth controller
		if( NCommManager::m_currentCommand.Equals( "deptloff" ) )
		{
			// Deactivate PID controller
			m_depthController.Deactivate();

			// Send command to drive lift at zero capacity (to turn off whatever was last sent by the controller)
			int m_argumentsToSend[] = { 1, 0 };
			NCommManager::m_currentCommand.PushCommand( "lift", m_argumentsToSend );

			Serial.println( F( "log:depth_hold_disabled;" ) );

			Serial.print( F( "targetDepth:" ) );
			Serial.print( DISABLED );
			Serial.println( ';' );

			return;
		}

		// Enable depth controller
		if( NCommManager::m_currentCommand.Equals( "deptlon" ) )
		{
			if( NCommManager::m_currentCommand.m_arguments[0] == 0 )
			{
				NDataManager::m_controllerData.depthSetpoint = NDataManager::m_controllerData.depth;
			}
			else
			{
				NDataManager::m_controllerData.depthSetpoint = NCommManager::m_currentCommand.m_arguments[1];
			}

			// Activate PID controller
			m_depthController.Activate();

			Serial.print( F( "log:depth_hold_enabled on=" ) );
			Serial.print( *m_depthController.m_pSetpoint );
			Serial.println( ';' );

			Serial.print( F( "targetDepth:" ) );
			Serial.print( *m_depthController.m_pSetpoint );
			Serial.println( ';' );

			return;
		}
	}
	void AllocateDepthControls()
	{
		if( m_depthController.m_isActive )
		{
			// Map from -1to1 to -100to100
			int lift = static_cast<int>( NDataManager::m_controllerData.depthCommand * 100.0f );

			// TODO: Perform any deadbanding if necessary

			Serial.println( F( "log:dhold pushing command;" ) );

			// TODO: Add error term to PID Controller class
			// TODO: Is this supposed to display as an integer?
			int error = static_cast<int>( NDataManager::m_controllerData.depthSetpoint - NDataManager::m_controllerData.depth );

			Serial.print( F( "dp_er:" ) );
			Serial.print( error );
			Serial.println( ';' );

			// Send lift command
			int m_argumentsToSend[] = { 1, lift };
			NCommManager::m_currentCommand.PushCommand( "lift", m_argumentsToSend );
		}
	}

	void AllocateYawControls()
	{
		if( m_yawController.m_isActive )
		{
			// Map from -1to1 to -100to100
			int yaw = static_cast<int>( NDataManager::m_controllerData.yawCommand * 50.0f );

			// TODO: Perform any deadbanding if necessary

			Serial.println( F( "log:dhold pushing command;" ) );

			// TODO: Add error term to PID Controller class
			// TODO: Is this supposed to display as an integer?
			int error = static_cast<int>( NORMALIZE_ANGLE( NDataManager::m_controllerData.yawSetpoint - NDataManager::m_controllerData.yaw ) );

			Serial.print( F( "dp_er:" ) );
			Serial.print( error );
			Serial.println( ';' );

			// Send yaw command
			int m_argumentsToSend[] = { 1, yaw };
			NCommManager::m_currentCommand.PushCommand( "yaw", m_argumentsToSend );
		}
	}

	void UpdateControllers()
	{
		// Compute controller outputs
		m_yawController.Compute();
		m_depthController.Compute();

		// Allocate controls
		AllocateDepthControls();
		AllocateYawControls();
	}
}

void CAutopilot::Initialize()
{
	// Initialize the controllers
	// TODO: Maybe rethink pointer access and keep it standalone for usability
	m_yawController.Initialize(	&NDataManager::m_controllerData.yaw, &NDataManager::m_controllerData.yawCommand, &NDataManager::m_controllerData.yawSetpoint );
	m_depthController.Initialize( &NDataManager::m_controllerData.depth, &NDataManager::m_controllerData.depthCommand, &NDataManager::m_controllerData.depthSetpoint );

	// Reset the timer
	pilotTimer.Reset();

	Serial.println( F( "log:pilot setup complete;" ) );
}

void CAutopilot::Update( CCommand& commandIn )
{
	// Handle commands
	HandleCommands();

	// Run controllers at 20Hz and send resulting motor control commands
	if( pilotTimer.HasElapsed( 50 ) )
	{
		// Update the controllers
		UpdateControllers();
	}
}



#endif
