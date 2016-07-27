
#include "SysConfig.h"
#if(HAS_BMP280)

#include <CI2C.h>
#include "CBMP280.h"
#include "CTimer.h"
#include "NDataManager.h"

namespace
{
	CTimer bmp280_sample_timer;
	CTimer init_timer;

	bool initialized = false;

	float pressure = 0.0f;
	float temperature = 0.0f;
}

CBMP280::CBMP280( CI2C *i2cInterfaceIn )
	: m_bmp( i2cInterfaceIn )
{
}

void CBMP280::Initialize()
{
	Serial.println( "BMP280.Status:INIT;" );

	//Reset timers
	bmp280_sample_timer.Reset();
	init_timer.Reset();

	Serial.println( "BMP280.Status:POSTINIT;" );
}

void CBMP280::InitializeSensor()
{
	// Attempt to initialize. If this fails, we try every 5 seconds in its update loop
	if( m_bmp.Initialize() )
	{
		Serial.println( "BMP_INIT_STATUS:FAILED;" );
	}
	else
	{
		Serial.println( "BMP_INIT_STATUS:SUCCESS;" );

		initialized = true;
	}
}



void CBMP280::Update( CCommand &commandIn )
{
	// 10hz
	if( bmp280_sample_timer.HasElapsed( 100 ) )
	{
		if( !initialized )
		{
			// Attempt every 10 secs
			if( init_timer.HasElapsed( 5000 ) )
			{
				Serial.println( "BMP280.Status:TRYING;" );

				// Attempt to initialize the chip again
				InitializeSensor();
			}

			return;
		}
		
		// Get pressure and temperature
		if( !m_bmp.ReadCompensatedPressureAndTemperature( pressure, temperature ) )
		{
			Serial.print( "BMP280.pres:" );
			Serial.print( pressure );
			Serial.println( ";" );

			Serial.print( "BMP280.temp:" );
			Serial.print( temperature );
			Serial.println( ";" );
		}
		else
		{
			// Some error occurred, restart.
			initialized = false;
		}
	}
}

#endif