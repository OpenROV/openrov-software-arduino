#include "SysConfig.h"
#if CONTROLLERBOARD == CONTROLLERBOARD_TRIDENT

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
	// Define the number of samples to keep track of.  The higher the number,
	// the more the readings will be smoothed, but the slower the output will
	// respond to the input.  Using a constant rather than a normal variable lets
	// use this value to determine the size of the readings array.
	const int numReadings 		= 1;
	int readings[numReadings]	= {0};      // the readings from the analog input

	CTimer timer1;
	CTimer onesecondtimer;
	CTimer statustime2;

	int ind						= 0;            // the ind of the current reading
	int total					= 0;            // the running total
	int average					= 0;			// the average

	float celsiusTempRead 		= 0.0f;

	CPin pin_enExtI2C( "pin_enExtI2C", PIN_ENABLE_EXT_I2C, CPin::kDigital, CPin::kOutput );
	CPin pin_enIntI2C( "pin_enIntI2C", PIN_ENABLE_INT_I2C, CPin::kDigital, CPin::kOutput );

// 	void readTemp()
// 	{
// 		float analogTempRead = analogRead( Pin_BoardTemp );

// 		float volt = util::mapf( analogTempRead * 1.0, 0, 1024, 0, 5.0 ); // change 4: 1024.0, otherwise will calc integer value!!
// 		celsiusTempRead = ( volt - .4 ) * 51.28;
// 	}

// 	float readCurrent( int pin )
// 	{
// 		int voltage = analogRead( pin );

// 		return util::mapf( voltage, 0, 1023, 0, 10 );
// 	}

// 	float read20Volts( int pin )
// 	{
// 		int voltage = analogRead( pin );

// 		return util::mapf( voltage, 0, 1023, 0, 20 );
// 	}

// 	float readBrdCurrent( int pin )
// 	{
// 		int voltage = analogRead( pin );

// 		return util::mapf( voltage, 0, 1023, 0, 2 );
// 	}

// 	float readVcc()
// 	{
// 		return 0.0f;
// 		// int voltage = analogRead( pin );

// 		// return mapf( voltage, 0, 1023, 0, 10 );
		
		
// // 		// Read 1.1V reference against AVcc
// // 		// set the reference to Vcc and the measurement to the internal 1.1V reference
// // #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// // 		ADMUX = _BV( REFS0 ) | _BV( MUX4 ) | _BV( MUX3 ) | _BV( MUX2 ) | _BV( MUX1 );
// // #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
// // 		ADMUX = _BV( MUX5 ) | _BV( MUX0 );
// // #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
// // 		ADMUX = _BV( MUX3 ) | _BV( MUX2 );
// // #else
// // 		ADMUX = _BV( REFS0 ) | _BV( MUX3 ) | _BV( MUX2 ) | _BV( MUX1 );
// // #endif

// // 		delay( 2 ); // Wait for Vref to settle
// // 		ADCSRA |= _BV( ADSC ); // Start conversion

// // 		while( bit_is_set( ADCSRA, ADSC ) ); // measuring

// // 		uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
// // 		uint8_t high = ADCH; // unlocks both

// // 		long result = ( high << 8 ) | low;

// // 		result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
// // 		return result; // Vcc in millivolts


	// }
}

void CControllerBoard::Initialize()
{
	// Reset timers
	timer1.Reset();
	statustime2.Reset();
	onesecondtimer.Reset();

	// i2cpower.Reset();
	// i2cpower.Write( 0 );
	// delay( 10 );
	// i2cpower.Write( 1 );

	// // Initialize all the readings to 0:
	// for( int thisReading = 0; thisReading < numReadings; ++thisReading )
	// {
	// 	readings[ thisReading ] = 0;
	// }
}

void CControllerBoard::Update( CCommand& commandIn )
{
	// if( time.HasElapsed( 100 ) )
	// {
	// 	// subtract the last reading:
	// 	total = total - readings[ind];
	// 	// read from the sensor:
	// 	readings[ind] = readBrdCurrent( A0 );

	// 	// add the reading to the total:
	// 	total = total + readings[ind];
	// 	// advance to the next position in the array:
	// 	ind = ind + 1;

	// 	// if we're at the end of the array...
	// 	if( ind >= numReadings )
	// 		// ...wrap around to the beginning:
	// 	{
	// 		ind = 0;
	// 	}

	// 	// calculate the average:
	// 	average = total / numReadings;
	// }

	if( onesecondtimer.HasElapsed( 1000 ) )
	{
		// readTemp();
		// Serial.print( F( "BRDT:" ) );
		// Serial.print( celsiusTempRead );
		// Serial.print( ';' );
		// Serial.print( F( "SC1I:" ) );
		// Serial.print( readCurrent( PIN_A13 ) );
		// Serial.print( ';' );
		// Serial.print( F( "SC2I:" ) );
		// Serial.print( readCurrent( PIN_A15 ) );
		// Serial.print( ';' );
		// Serial.print( F( "SC3I:" ) );
		// Serial.print( readCurrent( PIN_A3 ) );
		// Serial.print( ';' );
		// Serial.print( F( "BRDI:" ) );
		// Serial.print( readBrdCurrent( PIN_A2 ) );
		// Serial.print( ';' );
		// Serial.print( F( "BT1I:" ) );
		// Serial.print( readCurrent( PIN_A8 ) );
		// Serial.print( ';' );
		// Serial.print( F( "BT2I:" ) );
		// Serial.print( readCurrent( PIN_A9 ) );
		// Serial.print( ';' );
		// Serial.print( F( "BRDV:" ) );
		// Serial.print( read20Volts( PIN_A14 ) );
		// Serial.println( ';' );
		Serial.println( "ControllerBoard:1;" );
	}

	// // Update Cape Data voltages and currents
	// if( statustime2.HasElapsed( 100 ) )
	// {
	// 	NDataManager::m_capeData.VOUT = read20Volts( PIN_A14 );

	// 	// #315: deprecated: this is the same thing as BRDI:
	// 	NDataManager::m_capeData.IOUT = readBrdCurrent( PIN_A2 );

	// 	// Total current draw from batteries:
	// 	NDataManager::m_capeData.BTTI = readCurrent( PIN_A8 ) + readCurrent( PIN_A9 );
	// 	NDataManager::m_capeData.FMEM = 1; //util::FreeMemory();
	// 	NDataManager::m_capeData.UTIM = millis();
	// }
}

#endif