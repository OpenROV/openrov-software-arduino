#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_CAPE

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

	float ReadBoardTemperature()
	{
		unsigned int wADC;
        double t;

        // The internal temperature has to be used
        // with the internal reference of 1.1V.
        // Channel 8 can not be selected with
        // the analogRead function yet.

        // Set the internal reference and mux.
        ADMUX = ( _BV( REFS1 ) | _BV( REFS0 ) | _BV( MUX3 ) );
        ADCSRA |= _BV( ADEN ); // enable the ADC

        delay( 20 );          // wait for voltages to become stable.

        ADCSRA |= _BV( ADSC ); // Start the ADC

        // Detect end-of-conversion
        while( bit_is_set( ADCSRA, ADSC ) );

        // Reading register "ADCW" takes care of how to read ADCL and ADCH.
        wADC = ADCW;

        // The offset of 324.31 could be wrong. It is just an indication.
        t = ( wADC - 324.31 ) / 1.22;

        // The returned temperature is in degrees Celcius.
        return ( t );
	}

	float ReadVoltage_Board()
	{
		return util::mapf( (float)analogRead( PIN_BOARD_V ), 0.0f, 1023.0f, 0.0f, 50.0f );
	}

	float ReadCurrent_Board()
	{
		return util::mapf( (float)analogRead( PIN_BOARD_I ), 0.0f, 1023.0f, 0.0f, 5.0f ) + 0.4f;
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
		readings[ind] = ReadCurrent_Board();

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
		Serial.print( F( "BRDI:" ) );
		Serial.print( ReadCurrent_Board() );
		Serial.print( ';' );
		Serial.print( F( "BRDV:" ) );
		Serial.print( ReadVoltage_Board() );
		Serial.println( ';' );
	
		Serial.println( "ControllerBoard:1;" );
	}

	// Update Cape Data voltages and currents
	if( m_10hzTimer.HasElapsed( 100 ) )
	{
		NDataManager::m_capeData.VOUT = ReadVoltage_Board();
        NDataManager::m_capeData.IOUT = ReadCurrent_Board();
        NDataManager::m_capeData.FMEM = util::FreeMemory();
        NDataManager::m_capeData.ATMP = ReadBoardTemperature();
        NDataManager::m_capeData.UTIM = millis();
	}
}

#endif