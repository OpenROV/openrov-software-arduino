// Includes
#include "NDataManager.h"

namespace NDataManager
{
	// Default initialize all data
	TNavData			m_navData = {};
	TEnvironmentData	m_environmentData = {};
	TCapeData			m_capeData = {};
	TThrusterData		m_thrusterData = {};
	TCameraMountData	m_cameraMountData = {};
	TControllerData		m_controllerData = {};

	CTimer				m_timer_1hz;
	CTimer				m_timer_10hz;

	uint32_t			m_loopsPerSec = 0;

	// Called during Setup() to initialize any DataManager members to specific values
	void Initialize()
	{
		Serial.println( "Systems.DataManager.Status:INIT;" );

		Serial.println( "Systems.DataManager.Status:READY;" );
	}

	void OutputNavData()
	{
	}

	void OutputSharedData()
	{
	}

	void HandleOutputLoops()
	{
		++m_loopsPerSec;

		// 1Hz update loop
		if( m_timer_1hz.HasElapsed( 1000 ) )
		{
			// Send shared data to beaglebone
			OutputSharedData();

			// Loops per sec
			Serial.print( F( "alps:" ) );
			Serial.print( m_loopsPerSec );
			Serial.println( ';' );

			// Reset loop counter
			m_loopsPerSec = 0;
		}

		// 10Hz Update loop
		if( m_timer_10hz.HasElapsed( 100 ) )
		{
			// Send nav data to beaglebone
			OutputNavData();
		}
	}
}