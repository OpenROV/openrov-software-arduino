// Includes
#include "NSysManager.h"

#include <CI2C.h>
#include "PinDefinitions.h"

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
		// Set pin modes
		SetPinModes();
		
		// Enable LED0
		digitalWrite( PIN_LED_0, HIGH );
		
		// Initialize the serial ports
		InitializeSerial();

		Serial.println( "Systems.SysManager.Status:INIT;" );

		// Handle Watchdog startup
		DisableWatchdogTimer();
		EnableWatchdogTimer();

		// Initialize the I2C interfaces
		EnableI2C();

		// Check to see if we are recovering from a watchdog trigger
		CheckWatchdogLog();

		Serial.println( "Systems.SysManager.Status:READY;" );
	}
	
	void InitializeSerial()
	{
		Serial.begin( 115200 );		// Primary port
		Serial1.begin( 115200 );	// Debug port
	}

	void EnableI2C()
	{		
			// Note: The wire library will break if we attempt to use it with these low.
			digitalWrite( PIN_ENABLE_INT_I2C, HIGH );
			digitalWrite( PIN_ENABLE_EXT_I2C, HIGH );

			I2C0.Enable( 0, 0 );
			I2C1.Enable( 0, 0 );
	}
	
	void SetPinModes()
	{
		// Setup pinmodes for GPIO
		pinMode( PIN_LED_0,                 OUTPUT );
		pinMode( PIN_LED_1,                 OUTPUT );
		pinMode( PIN_ENABLE_INT_I2C,        OUTPUT );
		pinMode( PIN_ENABLE_EXT_I2C,        OUTPUT );
		pinMode( PIN_ENABLE_ESC,            OUTPUT );
		pinMode( PIN_ENABLE_ESC_PRECHARGE,  OUTPUT );
		pinMode( PIN_ENABLE_PROGRAM,        INPUT );
		
		pinMode( PIN_SERVO_1, 				OUTPUT );
		pinMode( PIN_SERVO_2, 				OUTPUT );
		pinMode( PIN_SERVO_3,				OUTPUT );
		pinMode( PIN_SERVO_4, 				OUTPUT );
		pinMode( PIN_SERVO_5, 				OUTPUT );
		pinMode( PIN_SERVO_6, 				OUTPUT );
		pinMode( PIN_SERVO_7, 				OUTPUT );
		pinMode( PIN_SERVO_8, 				OUTPUT );
	}

	void EnableWatchdogTimer()
	{
	}

	void DisableWatchdogTimer()
	{
	}

	void ResetWatchdogTimer()
	{
	}

	void CheckWatchdogLog()
	{
	}
}
