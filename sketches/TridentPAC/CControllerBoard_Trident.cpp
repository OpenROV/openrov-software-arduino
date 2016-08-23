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
	// Define the number of samples to keep track of. The higher the number, the more the readings will be smoothed, but the slower the output will respond to the input.
	const int numReadings 		= 1;
	float readings[numReadings]	= {0};      // the readings from the analog input

	CTimer m_averagedBoardCurrentTimer;
	CTimer m_1hzTimer;
	CTimer m_10hzTimer;

	int ind						= 0;            // the ind of the current reading
	int total					= 0;            // the running total
	int average					= 0;			// the average

	float r_pinVoltage			= 0.0f;			// Re-usable variable

	// TODO: All ranges, adc resolutions, etc should be configurable defines, not hardcoded

	// TODO: Finish finding out the board ranges

	float ReadVoltage_Batt1()
	{
		return 0.0f;
	}
	
	float ReadVoltage_Batt2()
	{
		return 0.0f;
	}
	
	float ReadVoltage_BattBus()
	{
		return 0.0f;
	}
	
	float ReadBoardTemperature()
	{
		// Map ADC range to voltage
		r_pinVoltage = util::mapf( (float)analogRead( PIN_BOARD_TEMP ), 0.0f, 1023.0f, 0.0f, 3.3f );
		
		// Convert voltage to Celsius
		return ( ( r_pinVoltage - .4f ) * 51.28f );
	}
	
	float ReadHumidity()
	{
		return 0.0f;
	}

	float ReadCurrent_NonESC()
	{
		return util::mapf( (float)analogRead( PIN_NON_ESC_I ), 0.0f, 1023.0f, 0.0f, 3.3f );;
	}

	float ReadCurrent_ESC1()
	{
		return util::mapf( (float)analogRead( PIN_ESC1_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}

	float ReadCurrent_ESC2()
	{
		return util::mapf( (float)analogRead( PIN_ESC2_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}

	float ReadCurrent_ESC3()
	{
		return util::mapf( (float)analogRead( PIN_ESC3_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}
	
	float ReadCurrent_BattTube1()
	{
		return util::mapf( (float)analogRead( PIN_BATT_TUBE1_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}
	
	float ReadCurrent_BattTube2()
	{
		return util::mapf( (float)analogRead( PIN_BATT_TUBE2_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}
	
	float ReadCurrent_ExtLoad()
	{
		return util::mapf( (float)analogRead( PIN_EXT_LOAD_I ), 0.0f, 1023.0f, 0.0f, 11.0f );;
	}
	
	float ReadTotalCurrent()
	{
		return ReadCurrent_BattTube1() + ReadCurrent_BattTube1();
	}
}

void CControllerBoard::Initialize()
{
	// Reset timers
	m_averagedBoardCurrentTimer.Reset();
	m_1hzTimer.Reset();
	m_10hzTimer.Reset();
}

void CControllerBoard::Update( CCommand& commandIn )
{
	// Averaged the board current
	if( m_averagedBoardCurrentTimer.HasElapsed( 100 ) )
	{
		// Subtract the last reading:
		total = total - readings[ind];
		
		// Read from the sensors
		readings[ind] = ReadTotalCurrent();

		// Add the reading to the total:
		total = total + readings[ind];
		
		// Advance to the next position in the array:
		ind = ind + 1;

		// If we're at the end of the array...
		if( ind >= numReadings )
		{
			// ...wrap around to the beginning:
			ind = 0;
		}

		// Calculate the average:
		average = total / numReadings;
	}

	if( m_1hzTimer.HasElapsed( 1000 ) )
	{
		Serial.print( F( "BRDT:" ) );
		Serial.print( ReadBoardTemperature() );
		Serial.print( ';' );
		Serial.print( F( "ESC1I:" ) );
		Serial.print( ReadCurrent_ESC1() );
		Serial.print( ';' );
		Serial.print( F( "ESC2I:" ) );
		Serial.print( ReadCurrent_ESC2() );
		Serial.print( ';' );
		Serial.print( F( "ESC3I:" ) );
		Serial.print( ReadCurrent_ESC3() );
		Serial.print( ';' );
		Serial.print( F( "BRDI:" ) );
		Serial.print( ReadTotalCurrent() );
		Serial.print( ';' );
		Serial.print( F( "BT1I:" ) );
		Serial.print( ReadCurrent_BattTube1() );
		Serial.print( ';' );
		Serial.print( F( "BT2I:" ) );
		Serial.print( ReadCurrent_BattTube2() );
		Serial.print( ';' );
		Serial.print( F( "BRDV:" ) );
		Serial.print( ReadVoltage_BattBus() );
		Serial.println( ';' );
	
		Serial.println( "ControllerBoard:1;" );
	}

	// Update Cape Data voltages and currents
	if( m_10hzTimer.HasElapsed( 100 ) )
	{
		NDataManager::m_capeData.VOUT = ReadVoltage_BattBus();

		// #315: deprecated: this is the same thing as BRDI:
		NDataManager::m_capeData.IOUT = ReadTotalCurrent();

		// Total current draw from batteries:
		NDataManager::m_capeData.BTTI = ReadTotalCurrent();
		NDataManager::m_capeData.FMEM = 1; //util::FreeMemory();
		NDataManager::m_capeData.UTIM = millis();
	}
}

#endif