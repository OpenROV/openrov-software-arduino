
#include "SysConfig.h"
#if(HAS_BNO055)

#include "CBNO055.h"
#include <BNO055.h>
#include "CTimer.h"
#include <Wire.h>

#include "NDataManager.h"

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
		if( !bno.m_isInitialized )
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

		// bno.GetCalibration();
		// bno.GetSystemStatus();
		// bno.GetSystemError();
		// bno.GetVector( bosch::EVectorType::VECTOR_EULER, euler );

		// yaw = fmod( euler.x() + 90.0f, 360.0f );

		// if( yaw < 0.0f )
		// {
		// 	yaw += 360.0f;
		// }


		// pitch		= euler.y();
		// roll		= euler.z();
		
		// System status checks
		if( report_timer.HasElapsed( 1000 ) )
		{
			// System calibration
			if( bno.GetCalibration() )
			{
				Serial.print( "BNO055.CALIB_MAG:" );
				Serial.print( bno.m_magCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_ACC:" );
				Serial.print( bno.m_accelCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_GYR:" );
				Serial.print( bno.m_gyroCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_SYS:" );
				Serial.print( bno.m_systemCal );
				Serial.println( ';' );

				// Get offsets
				bno.GetGyroOffsets();
				bno.GetAccelerometerOffsets();
				bno.GetMagnetometerOffsets();
			}
			else
			{
				Serial.print( "BNO055.CALIB_MAG:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_ACC:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_GYR:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_SYS:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
			}

			// Operating mode
			if( bno.GetOperatingMode() )
			{
				Serial.print( "BNO055.MODE:" );
				Serial.print( bno.m_operatingMode );
				Serial.println( ';' );
			}
			else
			{
				Serial.print( "BNO055.MODE:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
			}

			// System status
			if( bno.GetSystemStatus() )
			{
				Serial.print( "BNO055_STATUS:" );
				Serial.print( bno.m_systemStatus, HEX );
				Serial.println( ";" );
			}
			else
			{
				Serial.print( "BNO055_STATUS:" );
				Serial.print( "N/A" );
				Serial.println( ";" );
			}

			// System Error
			if( bno.GetSystemError() )
			{
				Serial.print( "BNO055_ERROR_FLAG:" );
				Serial.print( bno.m_systemError );
				Serial.println( ";" );
			}
			else
			{
				Serial.print( "BNO055_ERROR_FLAG:" );
				Serial.print( "N/A" );
				Serial.println( ";" );
			}

		}

		// Get orientation data
        if( bno.GetVector( bosch::VECTOR_EULER, euler ) )
        {			
            // Throw out exactly zero heading values that are all zeroes - necessary when switching modes
            if( euler.x() != 0.0f  )
            {
                // These may need adjusting
                
                NDataManager::m_navData.YAW		= euler.x();
                NDataManager::m_navData.HDGD	= euler.x();
            }
			
			NDataManager::m_navData.PITC	= euler.z();
			NDataManager::m_navData.ROLL	= -euler.y();
        }
	}
}

#endif