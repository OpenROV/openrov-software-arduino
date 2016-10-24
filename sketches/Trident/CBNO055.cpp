
#include "SysConfig.h"
#if(HAS_BNO055)

#include <CI2C.h>
#include "CBNO055.h"
#include "CTimer.h"
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

	imu::Vector<3> euler;

	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
}

CBNO055::CBNO055( CI2C *i2cInterfaceIn )
	: m_bno( i2cInterfaceIn )
{
}

void CBNO055::Initialize()
{
	Serial.println( "BNO055.Status:INIT;" );

	//Reset timers
	bno055_sample_timer.Reset();
	report_timer.Reset();
	imuTimer.Reset();
	fusionTimer.Reset();

	Serial.println( "BNO055.Status:POSTINIT;" );
}

void CBNO055::InitializeSensor()
{
	// Attempt to initialize. If this fails, we try every 30 seconds in its update loop
	int32_t ret = m_bno.Initialize();
	if( ret != 0 )
	{
		Serial.println( "BNO_INIT_STATUS:FAILED;" );
		Serial.print( "BNO_FAIL_REASON:" );
		Serial.print( ret );
		Serial.println( ";" );
	}
	else
	{
		Serial.println( "BNO_INIT_STATUS:SUCCESS;" );


		Serial.print( "BNO055.SW_Revision_ID:" );
		Serial.print( m_bno.m_softwareVersionMajor, HEX );
		Serial.print( "." );
		Serial.print( m_bno.m_softwareVersionMinor, HEX );
		Serial.println( ";" );

		Serial.print( "BNO055.bootloader:" );
		Serial.print( m_bno.m_bootloaderRev );
		Serial.println( ";" );

		inFusionMode = true;
	}
}



void CBNO055::Update( CCommand &commandIn )
{
	// 100hz
	if( bno055_sample_timer.HasElapsed( 10 ) )
	{
		if( !m_bno.m_isInitialized )
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
		
		// System status checks
		if( report_timer.HasElapsed( 1000 ) )
		{
			// System calibration
			if( m_bno.GetCalibration() == 0 )
			{
				Serial.print( "BNO055.CALIB_MAG:" );
				Serial.print( m_bno.m_magCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_ACC:" );
				Serial.print( m_bno.m_accelCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_GYR:" );
				Serial.print( m_bno.m_gyroCal );
				Serial.println( ';' );
				Serial.print( "BNO055.CALIB_SYS:" );
				Serial.print( m_bno.m_systemCal );
				Serial.println( ';' );

				// Get offsets
				m_bno.GetGyroOffsets();
				m_bno.GetAccelerometerOffsets();
				m_bno.GetMagnetometerOffsets();
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
			if( m_bno.GetOperatingMode() == 0 )
			{
				Serial.print( "BNO055.MODE:" );
				Serial.print( m_bno.m_operatingMode );
				Serial.println( ';' );
			}
			else
			{
				Serial.print( "BNO055.MODE:" );
				Serial.print( "N/A" );
				Serial.println( ';' );
			}

			// System status
			if( m_bno.GetSystemStatus() == 0 )
			{
				Serial.print( "BNO055_STATUS:" );
				Serial.print( m_bno.m_systemStatus, HEX );
				Serial.println( ";" );
			}
			else
			{
				Serial.print( "BNO055_STATUS:" );
				Serial.print( "N/A" );
				Serial.println( ";" );
			}

			// System Error
			if( m_bno.GetSystemError() == 0 )
			{
				Serial.print( "BNO055_ERROR_FLAG:" );
				Serial.print( m_bno.m_systemError );
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
        if( m_bno.GetVector( bno055::VECTOR_EULER, euler ) == 0 )
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