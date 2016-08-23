// Includes
#include "NSysManager.h"

#include <CI2C.h>
#include "PinDefinitions.h"

namespace NSysManager
{
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
		
		// Initialize the I2C interfaces
		EnableI2C();

		Serial.println( "Systems.SysManager.Status:READY;" );
	}
	
	void InitializeSerial()
	{
		Serial.begin( 115200 );
		SerialDebug.begin( 115200 );
		SerialMotor0.begin( 115200 );
		SerialMotor1.begin( 115200 );
		SerialMotor2.begin( 115200 );
	}

	void EnableI2C()
	{
		digitalWrite( PIN_ENABLE_I2C, HIGH );
		I2C0.Enable( 0, 0 );
	}
	
	void SetPinModes()
	{
		// Setup pinmodes for GPIO
		pinMode( PIN_LED_0,                 OUTPUT );
		pinMode( PIN_LED_1,                 OUTPUT );
		pinMode( PIN_ENABLE_I2C,        	OUTPUT );
		pinMode( PIN_ENABLE_ESC,            OUTPUT );
		
		pinMode( PIN_SERVO_0, 				OUTPUT );
		pinMode( PIN_SERVO_1, 				OUTPUT );
		pinMode( PIN_SERVO_2,				OUTPUT );
	}
}
