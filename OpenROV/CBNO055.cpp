#include "AConfig.h"
#if(HAS_BNO055)

#include "NDataManager.h"
#include "CBNO055.h"
#include "CAdaBNO055.h"
#include "CTimer.h"

namespace
{
	CTimer bno055_sample_timer;
	CTimer report_timer;
	CTimer imuTimer;

	bool initalized				= false;
	bool inFusionMode			= false;

	CAdaBNO055 bno;

	imu::Vector<3> euler;

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

void CBNO055::Initialize()
{
	// Reset timers
	bno055_sample_timer.Reset();
	report_timer.Reset();
	imuTimer.Reset();
	
	InitializeSensor();
}

void CBNO055::Update( CCommand& commandIn )
{
	if( commandIn.Equals( "imumode" ) )
	{
		if( initalized && commandIn.m_arguments[ 0 ] != 0 )
		{
			if( commandIn.m_arguments[ 1 ] == 0 )
			{
				// Turn off override
				inFusionMode	= true;
				bno.EnterNDOFMode();

			}

			if( commandIn.m_arguments[ 1 ] == 12 )
			{
				// Override to NDOF
				inFusionMode	= true;
				bno.EnterNDOFMode();
			}

			if( commandIn.m_arguments[ 1 ] == 8 )
			{
				// Override to IMU mode
				inFusionMode	= false;
				bno.EnterIMUMode();
			}
		}
		else
		{
			Serial.println( "log:Can't enter override, IMU is not initialized yet!;" );
		}
	}

	// 1000 / 21
	if( bno055_sample_timer.HasElapsed( 47 ) )
	{
		if( !initalized )
		{
			// Attempt every 10 secs
			if( report_timer.HasElapsed( 10000 ) )
			{
				// Attempt to initialize the chip again
				InitializeSensor();
			}

			return;
		}

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
        if( bno.GetVector( CAdaBNO055::VECTOR_EULER, euler ) )
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
