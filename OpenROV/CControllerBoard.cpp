#include "AConfig.h"
#if( HAS_OROV_CONTROLLERBOARD_25 )

// Includes
#include <Arduino.h>
#include "NDataManager.h"
#include "CControllerBoard.h"
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
	const int numReadings = 1;
	int readings[numReadings];      // the readings from the analog input

	CTimer time;
	CTimer onesecondtimer;
	CTimer statustime2;

	int index			= 0;            // the index of the current reading
	int total			= 0;            // the running total
	int average			= 0;			// the average
	int temppin			= A8;

	float celsiusTempRead;

	CPin i2cpower( "i2cpower", I2CPOWER_PIN, CPin::kDigital, CPin::kOutput );

	double GetTemp( void )
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

	float mapf( long x, long in_min, long in_max, long out_min, long out_max )
	{
		return ( float )( x - in_min ) * ( out_max - out_min ) / ( float )( in_max - in_min ) + out_min;
	}

	void readTemp()
	{
		float analogTempRead = analogRead( temppin );

		float volt = mapf( analogTempRead * 1.0, 0, 1024, 0, 5.0 ); // change 4: 1024.0, otherwise will calc integer value!!
		celsiusTempRead = ( volt - .4 ) * 51.28;
	}

	float readCurrent( int pin )
	{
		int voltage = analogRead( pin );

		return mapf( voltage, 0, 1023, 0, 10 );
	}

	float read20Volts( int pin )
	{
		int voltage = analogRead( pin );

		return mapf( voltage, 0, 1023, 0, 20 );
	}

	float readBrdCurrent( int pin )
	{
		int voltage = analogRead( pin );

		return mapf( voltage, 0, 1023, 0, 2 );
	}

	long readVcc()
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
	time.Reset();
	statustime2.Reset();
	onesecondtimer.Reset();

	i2cpower.Reset();
	i2cpower.Write( 0 );
	delay( 10 );
	i2cpower.Write( 1 );

	// Initialize all the readings to 0:
	for( int thisReading = 0; thisReading < numReadings; ++thisReading )
	{
		readings[ thisReading ] = 0;
	}
}

void CControllerBoard::Update( CCommand& commandIn )
{
	if( time.HasElapsed( 100 ) )
	{
		// subtract the last reading:
		total = total - readings[index];
		// read from the sensor:
		readings[index] = readBrdCurrent( A0 );

		// add the reading to the total:
		total = total + readings[index];
		// advance to the next position in the array:
		index = index + 1;

		// if we're at the end of the array...
		if( index >= numReadings )
			// ...wrap around to the beginning:
		{
			index = 0;
		}

		// calculate the average:
		average = total / numReadings;
	}

	if( onesecondtimer.HasElapsed( 1000 ) )
	{
		readTemp();
		Serial.print( F( "BRDT:" ) );
		Serial.print( celsiusTempRead );
		Serial.print( ';' );
		Serial.print( F( "SC1I:" ) );
		Serial.print( readCurrent( A3 ) );
		Serial.print( ';' );
		Serial.print( F( "SC2I:" ) );
		Serial.print( readCurrent( A2 ) );
		Serial.print( ';' );
		Serial.print( F( "SC3I:" ) );
		Serial.print( readCurrent( A1 ) );
		Serial.print( ';' );
		Serial.print( F( "BRDI:" ) );
		Serial.print( readBrdCurrent( A0 ) );
		Serial.print( ';' );
		Serial.print( F( "BT1I:" ) );
		Serial.print( readCurrent( A6 ) );
		Serial.print( ';' );
		Serial.print( F( "BT2I:" ) );
		Serial.print( readCurrent( A5 ) );
		Serial.print( ';' );
		Serial.print( F( "BRDV:" ) );
		Serial.print( read20Volts( A4 ) );
		Serial.print( ';' );
		Serial.print( F( "AVCC:" ) );
		Serial.print( readVcc() );
		Serial.println( ';' );

	}

	// Update Cape Data voltages and currents
	if( statustime2.HasElapsed( 100 ) )
	{
		NDataManager::m_capeData.VOUT = read20Volts( A4 );

		// #315: deprecated: this is the same thing as BRDI:
		NDataManager::m_capeData.IOUT = readBrdCurrent( A0 );

		// Total current draw from batteries:
		NDataManager::m_capeData.BTTI = readCurrent( A5 ) + readCurrent( A6 );
		NDataManager::m_capeData.FMEM = util::FreeMemory();
		NDataManager::m_capeData.UTIM = millis();
	}
}

#endif