#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_TRIDENT

// Includes
#include "CControllerBoard.h"

#include <Arduino.h>
#include "NDataManager.h"
#include "CTimer.h"
#include "Utility.h"

// File local variables and methods
namespace
{
	CTimer m_1hzTimer;
	CTimer m_10hzTimer;

	float m_boardVoltage		= 0.0f;
	float m_boardCurrent		= 0.0f;
	float m_esc0Current			= 0.0f;
	float m_esc1Current			= 0.0f;
	float m_esc2Current			= 0.0f;
	float m_totalCurrent		= 0.0f;

	float m_temp0				= 0.0f;
	float m_temp1				= 0.0f;
	float m_temp2				= 0.0f;
	float m_temp3				= 0.0f;
	float m_temp4				= 0.0f;
	float m_temp5				= 0.0f;
	float m_humidity			= 0.0f;

	inline float ReadTemperature( uint8_t pin )
	{
		return ( ( util::mapToRange( (float)analogRead( pin ), 0.0f, 1023.0f, 0.0f, 3.3f ) * 100.0f ) - 50.0f );
	}

	inline float ReadCurrent3_3A( uint8_t pin )
	{
		return 2.0f * util::mapToRange( (float)analogRead( pin ), 0.0f, 1023.0f, 0.0f, 3.3f );
	}

	inline float ReadCurrent10A( uint8_t pin )
	{
		return 2.0f * util::mapToRange( (float)analogRead( pin ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}

	inline float ReadVoltage20A( uint8_t pin )
	{
		return 2.0f * util::mapToRange( (float)analogRead( pin ), 0.0f, 1023.0f, 0.0f, 20.0f );
	}

	void ReadTemperatures()
	{
		m_temp0	= ReadTemperature( PIN_TEMP0 );
		m_temp1	= ReadTemperature( PIN_TEMP1 );
		m_temp2	= ReadTemperature( PIN_TEMP2 );
		m_temp3	= ReadTemperature( PIN_TEMP3 );
		m_temp4	= ReadTemperature( PIN_TEMP4 );
		m_temp5	= ReadTemperature( PIN_TEMP5 );
	}

	void ReadCurrents()
	{
		m_esc0Current = ReadCurrent10A( PIN_ESC0_I );
		m_esc1Current = ReadCurrent10A( PIN_ESC1_I );
		m_esc2Current = ReadCurrent10A( PIN_ESC2_I );
		m_boardCurrent = ReadCurrent3_3A( PIN_NONESC_I );
		m_totalCurrent = m_esc0Current + m_esc1Current + m_esc2Current + m_boardCurrent;
	}
	
	void ReadVoltages()
	{
		m_boardVoltage = ReadVoltage20A( PIN_BATT_V );
	}

	void ReadHumidity()
	{
		m_humidity = (float)analogRead( PIN_HUMIDITY );
	}
}

void CControllerBoard::Initialize()
{
	// Reset timers
	m_1hzTimer.Reset();
	m_10hzTimer.Reset();
}

void CControllerBoard::Update( CCommand& commandIn )
{
	// Update Cape Data voltages and currents
	if( m_10hzTimer.HasElapsed( 100 ) )
	{
		ReadTemperatures();
		ReadCurrents();
		ReadVoltages();
		ReadHumidity();

		NDataManager::m_capeData.VOUT = m_boardVoltage;

		// #315: deprecated: this is the same thing as BRDI:
		NDataManager::m_capeData.IOUT = m_boardCurrent;

		// Total current draw from batteries:
		NDataManager::m_capeData.BTTI = m_boardCurrent;
		NDataManager::m_capeData.FMEM = 1; //util::FreeMemory(); TODO: Implement FreeMemory for SAMD21
		NDataManager::m_capeData.UTIM = millis();
	}

	if( m_1hzTimer.HasElapsed( 1000 ) )
	{
		Serial.print( "TEMP0:" ); Serial.print( m_temp0 ); Serial.println( ';' );
		Serial.print( "TEMP1:" ); Serial.print( m_temp1 ); Serial.println( ';' );
		Serial.print( "TEMP2:" ); Serial.print( m_temp2 ); Serial.println( ';' );
		Serial.print( "TEMP3:" ); Serial.print( m_temp3 ); Serial.println( ';' );
		Serial.print( "TEMP4:" ); Serial.print( m_temp4 ); Serial.println( ';' );
		Serial.print( "BRDT:" ); Serial.print( m_temp5 ); Serial.println( ';' );
		Serial.print( "ESC0I:" ); Serial.print( m_esc0Current ); Serial.println( ';' );
		Serial.print( "ESC1I:" ); Serial.print( m_esc1Current ); Serial.println( ';' );
		Serial.print( "ESC2I:" ); Serial.print( m_esc2Current ); Serial.println( ';' );
		Serial.print( "BRDI:" ); Serial.print( m_boardCurrent ); Serial.println( ';' );
		Serial.print( "BRDV:" ); Serial.print( m_boardVoltage ); Serial.println( ';' );
		Serial.print( "HUMI:" ); Serial.print( m_humidity ); Serial.println( ';' );

		Serial.println( "ControllerBoard:1;" );
	}
}

#endif