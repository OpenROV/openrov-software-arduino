// Includes
#include "NSysManager.h"

#include <Wire.h>

#if MCUARCH == MCUARCH_AVR
	#include <EEPROM.h>
#endif

namespace NSysManager
{
	// ---------------------------------------------------------
	// Variable initialization
	// ---------------------------------------------------------
	
	volatile uint8_t m_wdtResetInfo	= 0;


	// ---------------------------------------------------------
	// Method Definitions
	// ---------------------------------------------------------
	
	void Initialize()
	{
		// Initialize the serial ports
		Serial.begin( 115200 );
		Serial1.begin( 115200 );
		
		Serial.println( "Systems.SysManager.Status:INIT;" );
		
		// Handle Watchdog startup
		DisableWatchdogTimer();
		EnableWatchdogTimer();
		
		// Initialize the I2C interfaces
		Wire.begin();
		Wire1.begin();

		// Check to see if we are recovering from a watchdog trigger
		CheckWatchdogLog();
		
		Serial.println( "Systems.SysManager.Status:READY;" );
	}

	void EnableWatchdogTimer()
	{
		#if MCUARCH == MCUARCH_AVR
			// Sets the watchdog timer both interrupt and reset mode with an 8 second timeout
			cli();
			MCUSR &= ~( 1 << WDRF );
			wdt_reset();
			WDTCSR |= ( 1 << WDCE ) | ( 1 << WDE );
			WDTCSR = ( ~( 1 << WDP1 ) & ~( 1 << WDP2 ) ) | ( ( 1 << WDE ) | ( 1 << WDIE ) | ( 1 << WDP3 ) | ( 1 << WDP0 ) );
			sei();
		#elif MCUARCH == MCUARCH_SAMD
			// Nothing yet
		#endif
	}

	void DisableWatchdogTimer()
	{
		#if MCUARCH == MCUARCH_AVR
			// Disables the watchdog timer
			cli();
			wdt_reset();
			MCUSR &= ~( 1 << WDRF );
			WDTCSR |= ( 1 << WDCE ) | ( 1 << WDE );
			WDTCSR = 0x00;
			sei();
		#elif MCUARCH == MCUARCH_SAMD
			// Nothing yet
		#endif
	}
	
	void ResetWatchdogTimer()
	{
		#if MCUARCH == MCUARCH_AVR
			wdt_reset();
		#elif MCUARCH == MCUARCH_SAMD
			// Nothing yet
		#endif
	}
	
	void CheckWatchdogLog()
	{
		#if MCUARCH == MCUARCH_AVR
			// Read first byte in EEPROM. If the watchdog triggered and the ISR completed, the first byte will be a "1"
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
		#elif MCUARCH == MCUARCH_SAMD
			// Nothing yet
		#endif
	}

#if MCUARCH == MCUARCH_AVR
	ISR( WDT_vect )
	{
		// Write the random number to the second byte
		EEPROM.write( 1, m_wdtResetInfo + 1 );

		// Write a "1" to the first byte to indicate the data in second byte is valid and the ISR triggered properly
		EEPROM.write( 0, 1 );

		// Triggers the second watchdog timeout for a reset
		while( true ) {};
	}
#endif

}
