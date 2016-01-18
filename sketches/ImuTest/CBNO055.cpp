
#include "Config.h"


#include "CBNO055.h"
#include "CAdaBNO055.h"
#include "CTimer.h"
#include <Wire.h>

namespace
{
	CTimer bno055_sample_timer;
	CTimer report_timer;
	CTimer imuTimer;
	CTimer fusionTimer;

	bool initalized				= false;
	bool browserPingReceived	= false;

	bool inFusionMode			= false;

	CAdaBNO055 bno( nullptr );

	imu::Vector<3> euler;

	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	void InitializeSensor()
	{
		// Attempt to initialize. If this fails, we try every 30 seconds in its update loop
		if( !bno.Initialize() )
		{
			Serial.println( "BNO_INIT_STATUS:FAILED;" );
			initalized = false;
		}
		else
		{
			Serial.println( "BNO_INIT_STATUS:SUCCESS;" );


			Serial.print( "BNO055.SW_Revision_ID:" );
			Serial.print( bno.m_softwareVersionMajor, HEX );
			Serial.print( "." );
			Serial.print( bno.m_softwareVersionMinor, HEX );
			Serial.println( ";" );

			Serial.print( "BNO055.bootloader:" );
			Serial.print( bno.m_bootloaderRev );
			Serial.println( ";" );

			initalized = true;
			inFusionMode = true;
		}
	}
}

CBNO055::CBNO055( TwoWire *wireInterfaceIn ) : CModule()
{
	bno.SetWireInterface( wireInterfaceIn );
}

void CBNO055::Initialize()
{
	// Reset timers
	bno055_sample_timer.Reset();
	report_timer.Reset();
	imuTimer.Reset();
	fusionTimer.Reset();
}



void CBNO055::Update()
{
	// 100hz
	if( bno055_sample_timer.HasElapsed( 10 ) )
	{
		if( !initalized )
		{
			// Attempt every 10 secs
			if( report_timer.HasElapsed( 5000 ) )
			{
				// Attempt to initialize the chip again
				InitializeSensor();
			}

			return;
		}

		bno.GetCalibration();
		bno.GetSystemStatus();
		bno.GetSystemError();
		bno.GetVector( CAdaBNO055::VECTOR_EULER, euler );
		

   
		yaw = fmod(euler.x() + 90.0f,360.0f);
		
    	if (yaw < 0.0f)
    	{
        	yaw += 360.0f;
    	}
		
		
		pitch		= euler.y();
		roll			= euler.z();

		// Serial.print(millis());
		// Serial.print('\t');
		// Serial.print(bno.m_systemStatus);
		// Serial.print('\t');
		// Serial.print(bno.m_systemError);
		// Serial.print('\t');
		// Serial.print(bno.m_systemCal);
		// Serial.print('\t');
		// Serial.print(bno.m_accelCal);
		// Serial.print('\t');
		// Serial.print(bno.m_gyroCal);
		// Serial.print('\t');
		Serial.print(bno.m_magCal);
		Serial.print('\t');
		Serial.print("BNO\t");
		Serial.print(roll, 2);
		Serial.print('\t');
		Serial.print(pitch, 2);
		Serial.print('\t');
		Serial.println(yaw, 2);
	}
}