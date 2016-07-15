/**********************************************************************************************
* Arduino PID Library - Version 1.1.1
* by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
*
* This Library is licensed under a GPLv3 License
*
* Modified by Charles Cross - OpenROV
**********************************************************************************************/

// Includes
#include "CPIDController.h"
#include "NDataManager.h"
#include "Utility.h"

CPIDController::CPIDController( float kpIn, float kiIn, float kdIn,
                                float outputMinIn, float outputMaxIn,
                                int8_t directionIn, uint32_t samplingPeriodIn_ms )
	: m_pInput( NULL )
	, m_pOutput( NULL )
	, m_pSetpoint( NULL )
	, m_lastTime_ms( 0 )
	, m_samplingPeriod_ms( samplingPeriodIn_ms )
	, m_controllerDirection( directionIn )
	, m_isActive( false )
	, m_isInitialized( false )
	, m_kp( 0.0f )
	, m_ki( 0.0f )
	, m_kd( 0.0f )
	, m_displayKp( kpIn )
	, m_displayKi( kiIn )
	, m_displayKd( kdIn )
	, m_pTerm( 0.0f )
	, m_iTerm( 0.0f )
	, m_dTerm( 0.0f )
	, m_lastInput( 0.0f )
	, m_outputMin( outputMinIn )
	, m_outputMax( outputMaxIn )
{

	// Calculate tuning parameters that will be used internally based on sampling period
	float sampleTime_s = ( ( float )m_samplingPeriod_ms ) / 1000.0f;

	m_kp = m_displayKp;
	m_ki = m_displayKi * sampleTime_s;
	m_kd = m_displayKd / sampleTime_s;

	// Handle direction
	if( m_controllerDirection == PID_CONTROLLER_DIRECTION_INVERSE )
	{
		m_kp = ( 0 - m_kp );
		m_ki = ( 0 - m_ki );
		m_kd = ( 0 - m_kd );
	}

	// Make sure output min is less than or equal to output max
	if( m_outputMin > m_outputMax )
	{
		m_outputMin = m_outputMax;
	}
}

void CPIDController::Activate()
{
	if( m_isActive )
	{
		// Already active!
		return;
	}

	// Handle smooth transition back to active state
	m_iTerm		= *m_pOutput;
	m_lastInput = *m_pInput;

	if( m_iTerm > m_outputMax )
	{
		m_iTerm = m_outputMax;
	}
	else if( m_iTerm < m_outputMin )
	{
		m_iTerm = m_outputMin;
	}

	m_isActive = true;
}


void CPIDController::Deactivate()
{
	m_isActive = false;
}

void CPIDController::Initialize( float* inputIn, float* outputIn, float* setpointIn )
{
	// Validate pointers
	if( !inputIn || !outputIn || !setpointIn )
	{
		// Nullpointer failure
		return;
	}

	// Set pointers
	m_pInput = inputIn;
	m_pOutput = outputIn;
	m_pSetpoint = setpointIn;

	// Controller is now initialized with valid source data (not necessarily active though!)
	m_isInitialized = true;
}

void CPIDControllerAngular::Compute()
{
	if( !m_isActive || !m_isInitialized )
	{
		return;
	}

	uint32_t now = millis();
	uint32_t timeChange = ( now - m_lastTime_ms );

	if( timeChange >= m_samplingPeriod_ms )
	{
		NDataManager::m_controllerData.yawError	= *m_pSetpoint - *m_pInput;
		float dInput	= NORMALIZE_ANGLE( *m_pInput - m_lastInput );

		// Update pTerm
		m_pTerm	= m_kp *  NDataManager::m_controllerData.yawError;

		// Update iTerm
		m_iTerm	+= ( m_ki *  NDataManager::m_controllerData.yawError );

		// Clamp iTerm
		if( m_iTerm > m_outputMax )
		{
			m_iTerm = m_outputMax;
		}
		else if( m_iTerm < m_outputMin )
		{
			m_iTerm = m_outputMin;
		}

		// Update dTerm
		m_dTerm = m_kd * dInput;

		// Compute PID output
		float output = m_pTerm + m_iTerm - m_dTerm;

		// Clamp overall output
		if( output > m_outputMax )
		{
			output = m_outputMax;
		}
		else if( output < m_outputMin )
		{
			output = m_outputMin;
		}

		// Set the output
		*m_pOutput		= output;

		// Track these variables for the next loop
		m_lastInput		= *m_pInput;
		m_lastTime_ms	= now;
	}
}

void CPIDControllerLinear::Compute()
{
	if( !m_isActive || !m_isInitialized )
	{
		return;
	}

	uint32_t now = millis();
	uint32_t timeChange = ( now - m_lastTime_ms );

	if( timeChange >= m_samplingPeriod_ms )
	{
		NDataManager::m_controllerData.yawError	= *m_pSetpoint - *m_pInput;



		float dInput	= ( *m_pInput - m_lastInput );

		// Update pTerm
		m_pTerm	= m_kp * NDataManager::m_controllerData.yawError;

		// Update iTerm
		m_iTerm	+= ( m_ki * NDataManager::m_controllerData.yawError );

		// Clamp iTerm
		if( m_iTerm > m_outputMax )
		{
			m_iTerm = m_outputMax;
		}
		else if( m_iTerm < m_outputMin )
		{
			m_iTerm = m_outputMin;
		}

		// Update dTerm
		m_dTerm = m_kd * dInput;

		// Compute PID output
		float output = m_pTerm + m_iTerm - m_dTerm;

		// Clamp overall output
		if( output > m_outputMax )
		{
			output = m_outputMax;
		}
		else if( output < m_outputMin )
		{
			output = m_outputMin;
		}

		// Set the output
		*m_pOutput		= output;

		// Track these variables for the next loop
		m_lastInput		= *m_pInput;
		m_lastTime_ms	= now;
	}
}