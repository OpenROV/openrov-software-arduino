// Includes
#include "NArduinoManager.h"

#include <Wire.h>
#include <EEPROM.h>

namespace NArduinoManager
{
	// Variable initialization
	volatile uint8_t m_wdtResetInfo	= 0;

	void Initialize()
	{
		// Hardware intialization
		DisableWatchdogTimer();
		EnableWatchdogTimer();

		// Initialize the serial port
		Serial.begin( 115200 );

		// Set the i2c to operate at 400khz (clock divisor value)
		TWBR = 12;

		// // Read first byte in EEPROM. If the watchdog triggered and the ISR completed, the first byte will be a "1"
		if( EEPROM.read( 0 ) == 1 )
		{
			// Read the second byte in the EEPROM
			m_wdtResetInfo = EEPROM.read( 1 );

			// Reset byte so the EEPROM is not read on next startup
			EEPROM.write( 0, 0 );

			// Print debug information about the watchdog triggering
			Serial.println( F( "log:Watchdog was triggered and the following was read from EEPROM;" ) );
			Serial.print( F( "log:" ) );
			Serial.println( m_wdtResetInfo );
			Serial.print( ';' );
		}

		// Set pin 13 as an output
		pinMode( 13, OUTPUT );
	}

	void EnableWatchdogTimer()
	{
		// Sets the watchdog timer both interrupt and reset mode with an 8 second timeout

		cli();
		MCUSR &= ~( 1 << WDRF );
		wdt_reset();
		WDTCSR |= ( 1 << WDCE ) | ( 1 << WDE );
		WDTCSR = ( ~( 1 << WDP1 ) & ~( 1 << WDP2 ) ) | ( ( 1 << WDE ) | ( 1 << WDIE ) | ( 1 << WDP3 ) | ( 1 << WDP0 ) );
		sei();
	}

	void DisableWatchdogTimer()
	{
		// Disables the watchdog timer

		cli();
		wdt_reset();
		MCUSR &= ~( 1 << WDRF );
		WDTCSR |= ( 1 << WDCE ) | ( 1 << WDE );
		WDTCSR = 0x00;
		sei();
	}

	void ScanI2C()
	{
		byte error;
		byte address;
		int nModules = 0;

		Serial.println( F( "log:Scanning...;" ) );

		for( address = 1; address < 127; address++ )
		{
			// The i2c_scanner uses the return value of the Write.endTransmisstion to see if a module did acknowledge to the address.
			Wire.beginTransmission( address );
			error = Wire.endTransmission();

			if( error == 0 )
			{
				Serial.print( F( "log:I2C module found at address 0x" ) );

				if( address < 16 )
				{
					Serial.print( "0" );
				}

				Serial.print( address, HEX );
				Serial.println( "  !;" );

				nModules++;
			}
			else if( error == 4 )
			{
				Serial.print( F( "log:Unknow error at address 0x" ) );

				if( address < 16 )
				{
					Serial.print( "0" );
				}

				Serial.print( address, HEX );
				Serial.println( ";" );
			}
		}

		if( nModules == 0 )
		{
			Serial.println( F( "log:No I2C modules found\n;" ) );
		}
		else
		{
			Serial.println( F( "log:done\n;" ) );
		}

		delay( 5000 );         // wait 5 seconds for next scan
	}

	ISR( WDT_vect )
	{
		// Write the random number to the second byte
		EEPROM.write( 1, m_wdtResetInfo + 1 );

		// Write a "1" to the first byte to indicate the data in second byte is valid and the ISR triggered properly
		EEPROM.write( 0, 1 );

		// Triggers the second watchdog timeout for a reset
		while( true ) {};
	}
}
