// Includes
#include "NDataManager.h"

namespace NDataManager
{
	// Default initialize all data
	TNavData			m_navData = {};
	TEnvironmentData	m_environmentData = {};
	TCapeData			m_capeData = {};
	TThrusterData		m_thrusterData = {};
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
		// Print Nav Data on the serial line
		Serial.print( F( "hdgd:" ) );
		Serial.print( m_navData.HDGD );
		Serial.print( ';' );
		Serial.print( F( "deep:" ) );
		Serial.print( m_navData.DEEP );
		Serial.print( ';' );
		Serial.print( F( "pitc:" ) );
		Serial.print( m_navData.PITC );
		Serial.print( ';' );
		Serial.print( F( "roll:" ) );
		Serial.print( m_navData.ROLL );
		Serial.print( ';' );
		Serial.print( F( "yaw:" ) );
		Serial.print( m_navData.YAW );
		Serial.print( ';' );
		Serial.print( F( "fthr:" ) );
		Serial.print( m_navData.FTHR );
		Serial.println( ';' );
	}

	void OutputSharedData()
	{
		// Print all other shared data on the serial line

		// Thruster Data
		Serial.print( F( "motorAttached:" ) );
		Serial.print( m_thrusterData.MATC );
		Serial.println( ';' );

		// Cape Data
		Serial.print( F( "fmem:" ) );
		Serial.print( m_capeData.FMEM );
		Serial.print( ';' );
		Serial.print( F( "vout:" ) );
		Serial.print( m_capeData.VOUT );
		Serial.print( ';' );
		Serial.print( F( "iout:" ) );
		Serial.print( m_capeData.IOUT );
		Serial.print( ';' );
		Serial.print( F( "btti:" ) );
		Serial.print( m_capeData.BTTI );
		Serial.print( ';' );
		Serial.print( F( "time:" ) );
		Serial.print( m_capeData.UTIM );
		Serial.println( ';' );

		// Environment Data
		Serial.print( F( "pres:" ) );
		Serial.print( m_environmentData.PRES );
		Serial.print( ';' );
		Serial.print( F( "temp:" ) );
		Serial.print( m_environmentData.TEMP );
		Serial.println( ';' );

		// Module loop timers in milliseconds
		// Serial.print( F( "modtime:" ) );

		// for( int i = 0; i < NModuleManager::m_moduleCount; ++i )
		// {
		// 	Serial.print( NModuleManager::m_pModules[ i ]->m_name );
		// 	Serial.print( '|' );
		// 	Serial.print( NModuleManager::m_pModules[ i ]->m_executionTime );
		// 	Serial.print( '|' );
		// }

		// Serial.println( ';' );
		
		//
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