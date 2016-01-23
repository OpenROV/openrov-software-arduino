
#include "SysConfig.h"
#if(HAS_BNO055)

#include "CBNO055.h"
#include <BNO055.h>
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

	// Use the first Wire interface by default
	CBNO055Driver bno;

	imu::Vector<3> euler;

	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	int m_wireInterfaceSelector = 0;
	TwoWire *m_pWire = nullptr;


	void InitializeSensor()
	{
		// Attempt to initialize. If this fails, we try every 30 seconds in its update loop
		if( !bno.Initialize( m_pWire ) )
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

// CBNO055::CBNO055( int wireInterfaceSelectorIn )
// {
// 	m_wireInterfaceSelector = wireInterfaceSelectorIn;
// }

void CBNO055::Initialize()
{
	Serial.println( "BNO055.Status:INIT;" );

	if( m_wireInterfaceSelector > WIRE_INTERFACES_COUNT )
	{
		// Invalid wire interface selected
		Serial.println( "ERROR:Invalid wire interface selected for BNO055;" );
		m_pWire = nullptr;
	}

	switch( m_wireInterfaceSelector )
	{
		case BNO_WIRE_INTERFACE_0:
			{
				m_pWire = WIRE_INTERFACE_0;
				break;
			}

		case BNO_WIRE_INTERFACE_1:
			{
				m_pWire = WIRE_INTERFACE_1;
				break;
			}

		case BNO_WIRE_INTERFACE_2:
			{
				m_pWire = WIRE_INTERFACE_2;
				break;
			}

		case BNO_WIRE_INTERFACE_3:
			{
				m_pWire = WIRE_INTERFACE_3;
				break;
			}

		case BNO_WIRE_INTERFACE_4:
			{
				m_pWire = WIRE_INTERFACE_4;
				break;
			}

		case BNO_WIRE_INTERFACE_5:
			{
				m_pWire = WIRE_INTERFACE_5;
				break;
			}

		default:
			m_pWire = nullptr;
			break;
	}

	//Reset timers
	bno055_sample_timer.Reset();
	report_timer.Reset();
	imuTimer.Reset();
	fusionTimer.Reset();

	Serial.println( "BNO055.Status:POSTINIT;" );
}



void CBNO055::Update( CCommand &commandIn )
{
	// 100hz
	if( bno055_sample_timer.HasElapsed( 10 ) )
	{
		if( !initalized )
		{
			// Attempt every 10 secs
			if( report_timer.HasElapsed( 5000 ) )
			{
				Serial.println( "BNO055.Status:TRYING;" );
				// Attempt to initialize the chip again
				InitializeSensor();
			}

			return;
		}

		bno.GetCalibration();
		bno.GetSystemStatus();
		bno.GetSystemError();
		bno.GetVector( bosch::EVectorType::VECTOR_EULER, euler );

		yaw = fmod( euler.x() + 90.0f, 360.0f );

		if( yaw < 0.0f )
		{
			yaw += 360.0f;
		}


		pitch		= euler.y();
		roll		= euler.z();

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
		// Serial.print(bno.m_magCal);
		// Serial.print('\t');
		// Serial.print("BNO\t");
		// Serial.print(roll, 2);
		// Serial.print('\t');
		// Serial.print(pitch, 2);
		// Serial.print('\t');
		// Serial.println(yaw, 2);
	}
}

#endif