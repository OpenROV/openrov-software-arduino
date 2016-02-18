#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_CB25

// Includes
#include "CControllerBoard.h"

#include <Arduino.h>
#include "NDataManager.h"
#include "CTimer.h"
#include "Utility.h"
#include "CPin.h"

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
	
	CPin m_pinEnI2C( "i2cpower", PIN_ENABLE_I2C, CPin::kDigital, CPin::kOutput );

	// TODO: All ranges, adc resolutions, etc should be configurable defines, not hardcoded

	// TODO: Finish finding out the board ranges

	float ReadBoardTemperature()
	{
		// Map ADC range to voltage
		r_pinVoltage = util::mapf( (float)analogRead( PIN_BOARD_TEMP ), 0.0f, 4095.0f, 0.0f, 3.3f );
		
		// Convert voltage to Celsius
		return ( ( r_pinVoltage - .4f ) * 51.28f );
	}

	float ReadVoltage_Board()
	{
		return util::mapf( (float)analogRead( PIN_BOARD_V ), 0.0f, 1023.0f, 0.0f, 20.0f );
	}

	float ReadCurrent_Board()
	{
		return util::mapf( (float)analogRead( PIN_BOARD_I ), 0.0f, 1023.0f, 0.0f, 2.0f );
	}

	float ReadCurrent_ESC1()
	{
		return util::mapf( (float)analogRead( PIN_ESC1_I ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}

	float ReadCurrent_ESC2()
	{
		return util::mapf( (float)analogRead( PIN_ESC2_I ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}

	float ReadCurrent_ESC3()
	{
		return util::mapf( (float)analogRead( PIN_ESC3_I ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}
	
	float ReadCurrent_BattTube1()
	{
		return util::mapf( (float)analogRead( PIN_BATT_TUBE1_I ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}
	
	float ReadCurrent_BattTube2()
	{
		return util::mapf( (float)analogRead( PIN_BATT_TUBE2_I ), 0.0f, 1023.0f, 0.0f, 10.0f );
	}
	
	float ReadTotalCurrent()
	{
		return ReadCurrent_BattTube1() + ReadCurrent_BattTube1();
	}
	
	long ReadVcc()
	{
		// Read 1.1V reference against AVcc
		// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		ADMUX = _BV( REFS0 ) | _BV( MUX4 ) | _BV( MUX3 ) | _BV( MUX2 ) | _BV( MUX1 );
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
		ADMUX = _BV( MUX5 ) | _BV( MUX0 );
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		ADMUX = _BV( MUX3 ) | _BV( MUX2 );
#else
		ADMUX = _BV( REFS0 ) | _BV( MUX3 ) | _BV( MUX2 ) | _BV( MUX1 );
#endif

		delay( 2 ); // Wait for Vref to settle
		ADCSRA |= _BV( ADSC ); // Start conversion

		while( bit_is_set( ADCSRA, ADSC ) ); // measuring

		uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
		uint8_t high = ADCH; // unlocks both

		long result = ( high << 8 ) | low;

		result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
		return result; // Vcc in millivolts
	}
}

void CControllerBoard::Initialize()
{
	// Reset timers
	m_averagedBoardCurrentTimer.Reset();
	m_1hzTimer.Reset();
	m_10hzTimer.Reset();
	
	// Reset I2C interface
	m_pinEnI2C.Reset();
	m_pinEnI2C.Write( 0 );
	delay( 10 );
	m_pinEnI2C.Write( 1 );
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
		Serial.print( ReadVoltage_Board() );
		Serial.print( ';' );
		Serial.print( F( "AVCC:" ) );
		Serial.print( ReadVcc() );
		Serial.println( ';' );
	
		Serial.println( "ControllerBoard:1;" );
	}

	// Update Cape Data voltages and currents
	if( m_10hzTimer.HasElapsed( 100 ) )
	{
		NDataManager::m_capeData.VOUT = ReadVoltage_Board();

		// #315: deprecated: this is the same thing as BRDI:
		NDataManager::m_capeData.IOUT = ReadTotalCurrent();

		// Total current draw from batteries:
		NDataManager::m_capeData.BTTI = ReadTotalCurrent();
		NDataManager::m_capeData.FMEM = util::FreeMemory();
		NDataManager::m_capeData.UTIM = millis();
	}
}

#endif