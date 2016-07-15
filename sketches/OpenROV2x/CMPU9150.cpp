// Includes
#include "CMPU9150.h"
#include "AConfig.h"

#if( HAS_MPU9150 )

#include <EEPROM.h>

#include "LibMPU.h"
#include "LibMPU_Calibration.h"

#include "NConfigManager.h"
#include "NDataManager.h"
#include "CModule.h"
#include "CTimer.h"

namespace
{
	MPU9150Lib MPU;

	int MPUModuleId = 1;

	boolean DidInit = false;
	boolean InCallibrationMode = false;

	CTimer MPU9150ReInit;
	CTimer mpu_update_timer;
	CTimer calibration_timer;

	CALLIB_DATA calData;

	int counter = 0;

	//  MPU_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the sensor data and DMP output
#define MPU_UPDATE_RATE  (20)

	//  MAG_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the magnetometer data
	//  MAG_UPDATE_RATE should be less than or equal to the MPU_UPDATE_RATE
#define MAG_UPDATE_RATE  (10)

	//  MPU_MAG_MIX defines the influence that the magnetometer has on the yaw output.
	//  The magnetometer itself is quite noisy so some mixing with the gyro yaw can help
	//  significantly. Some example values are defined below:
#define  MPU_MAG_MIX_GYRO_ONLY          0                   // just use gyro yaw
#define  MPU_MAG_MIX_MAG_ONLY           1                   // just use magnetometer and no gyro yaw
#define  MPU_MAG_MIX_GYRO_AND_MAG       10                  // a good mix value
#define  MPU_MAG_MIX_GYRO_AND_SOME_MAG  50                  // mainly gyros with a bit of mag correction

	//  MPU_LPF_RATE is the low pas filter rate and can be between 5 and 188Hz
#define MPU_LPF_RATE   5

}

void CMPU9150::Initialize()
{
	//Todo: Read calibration values from EPROM
	MPU.selectDevice( MPUModuleId );

	if( !MPU.init( MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE ) )
	{
		Serial.println( F( "log:Trying other MPU9150 address to init;" ) );
		Serial.print( F( "log:IMU Address was :" ) );
		Serial.print( 1 );
		MPUModuleId = !MPUModuleId;
		Serial.print( F( " but is now:" ) );
		Serial.print( MPUModuleId );
		Serial.println( ";" );
		MPU.selectDevice( MPUModuleId );

		if( MPU.init( MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE ) )
		{
			DidInit = true;
			Serial.println( F( "log:Init worked the second time;" ) );
		}
		else
		{
			Serial.println( F( "log:Failed to init on both addresses;" ) );
		}
	}
	else
	{
		DidInit = true;
		Serial.println( F( "log:init on primary addresses;" ) );
	}                             // start the MPU

	NConfigManager::m_capabilityBitmask |= ( 1 << COMPASS_CAPABLE );
	NConfigManager::m_capabilityBitmask |= ( 1 << ORIENTATION_CAPABLE );

	MPU9150ReInit.Reset();
	mpu_update_timer.Reset();
}

void CMPU9150::Update( CCommand& commandIn )
{
	if( !DidInit )
	{
		if( MPU9150ReInit.HasElapsed( 30000 ) )
		{
			CMPU9150::Initialize();
		}

		return;
	}

	if( commandIn.Equals( "ccal" ) )
	{
		// Compass_Calibrate();
		// The IMU needs both Magnatrometer and Acceleromter to be calibrated. This attempts to do them both at the same time
		calLibRead( MPUModuleId, &calData );             // pick up existing accel data if there

		calData.accelValid = false;
		calData.accelMinX = 0x7fff;                              // init accel cal data
		calData.accelMaxX = 0x8000;
		calData.accelMinY = 0x7fff;
		calData.accelMaxY = 0x8000;
		calData.accelMinZ = 0x7fff;
		calData.accelMaxZ = 0x8000;

		calData.magValid = false;
		calData.magMinX = 0x7fff;                                // init mag cal data
		calData.magMaxX = 0x8000;
		calData.magMinY = 0x7fff;
		calData.magMaxY = 0x8000;
		calData.magMinZ = 0x7fff;
		calData.magMaxZ = 0x8000;

		MPU.useAccelCal( false );
		//MPU.init(MPU_UPDATE_RATE, 5, 1, MPU_LPF_RATE);

		counter = 359;
		InCallibrationMode = true;
		calibration_timer.Reset();
		Serial.println( F( "!!!:While the compass counts down from 360 to 0, rotate the ROV slowly in all three axis;" ) );

	}

	if( InCallibrationMode )
	{
		bool changed = false;


		if( counter > 0 )
		{
			if( MPU.read() )                                         // get the latest data
			{
				changed = false;

				if( MPU.m_rawAccel[VEC3_X] < calData.accelMinX )
				{
					calData.accelMinX = MPU.m_rawAccel[VEC3_X];
					changed = true;
				}

				if( MPU.m_rawAccel[VEC3_X] > calData.accelMaxX )
				{
					calData.accelMaxX = MPU.m_rawAccel[VEC3_X];
					changed = true;
				}

				if( MPU.m_rawAccel[VEC3_Y] < calData.accelMinY )
				{
					calData.accelMinY = MPU.m_rawAccel[VEC3_Y];
					changed = true;
				}

				if( MPU.m_rawAccel[VEC3_Y] > calData.accelMaxY )
				{
					calData.accelMaxY = MPU.m_rawAccel[VEC3_Y];
					changed = true;
				}

				if( MPU.m_rawAccel[VEC3_Z] < calData.accelMinZ )
				{
					calData.accelMinZ = MPU.m_rawAccel[VEC3_Z];
					changed = true;
				}

				if( MPU.m_rawAccel[VEC3_Z] > calData.accelMaxZ )
				{
					calData.accelMaxZ = MPU.m_rawAccel[VEC3_Z];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_X] < calData.magMinX )
				{
					calData.magMinX = MPU.m_rawMag[VEC3_X];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_X] > calData.magMaxX )
				{
					calData.magMaxX = MPU.m_rawMag[VEC3_X];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_Y] < calData.magMinY )
				{
					calData.magMinY = MPU.m_rawMag[VEC3_Y];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_Y] > calData.magMaxY )
				{
					calData.magMaxY = MPU.m_rawMag[VEC3_Y];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_Z] < calData.magMinZ )
				{
					calData.magMinZ = MPU.m_rawMag[VEC3_Z];
					changed = true;
				}

				if( MPU.m_rawMag[VEC3_Z] > calData.magMaxZ )
				{
					calData.magMaxZ = MPU.m_rawMag[VEC3_Z];
					changed = true;
				}

				if( changed )
				{
					Serial.print( F( "dia:accel.MinX=" ) );
					Serial.print( calData.accelMinX );
					Serial.println( ";" );
					Serial.print( F( "dia:accel.maxX=" ) );
					Serial.print( calData.accelMaxX );
					Serial.println( ";" );
					Serial.print( F( "dia:accel.minY=" ) );
					Serial.print( calData.accelMinY );
					Serial.println( ";" );
					Serial.print( F( "dia:accel.maxY=" ) );
					Serial.print( calData.accelMaxY );
					Serial.println( ";" );
					Serial.print( F( "dia:accel.minZ=" ) );
					Serial.print( calData.accelMinZ );
					Serial.println( ";" );
					Serial.print( F( "dia:accel.maxZ=" ) );
					Serial.print( calData.accelMaxZ );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.minX=" ) );
					Serial.print( calData.magMinX );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.maxX=" ) );
					Serial.print( calData.magMaxX );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.minY=" ) );
					Serial.print( calData.magMinY );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.maxY=" ) );
					Serial.print( calData.magMaxY );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.minZ=" ) );
					Serial.print( calData.magMinZ );
					Serial.println( ";" );
					Serial.print( F( "dia:mag.maxZ=" ) );
					Serial.print( calData.magMaxZ );
					Serial.println( ";" );
				}
			}

			if( calibration_timer.HasElapsed( 1000 ) )
			{
				counter--;
				NDataManager::m_navData.HDGD = counter;
				Serial.print( F( "hdgd:" ) );
				Serial.print( NDataManager::m_navData.HDGD );
				Serial.print( ';' );
			}
		}

		if( counter <= 0 )
		{
			calData.accelValid = true;
			calData.magValid = true;
			calLibWrite( MPUModuleId, &calData );
			Serial.println( F( "log:Accel cal data saved for module;" ) );
			InCallibrationMode = false;
		}

		return;  //prevents the normal read and reporting of IMU data
	}
	else if( commandIn.Equals( "i2cscan" ) )
	{
		// scan();
	}

	//20 hz
	if( mpu_update_timer.HasElapsed( 50 ) )
	{
		if( MPU.read() )
		{
			NDataManager::m_navData.HDGD = MPU.m_fusedEulerPose[VEC3_Z] * RAD_TO_DEGREE;

			//To convert to-180/180 to 0/360
			if( NDataManager::m_navData.HDGD < 0 )
			{
				NDataManager::m_navData.HDGD += 360;
			}

			NDataManager::m_navData.PITC = MPU.m_fusedEulerPose[VEC3_Y] * RAD_TO_DEGREE;
			NDataManager::m_navData.ROLL = MPU.m_fusedEulerPose[VEC3_X] * RAD_TO_DEGREE;
			NDataManager::m_navData.YAW = MPU.m_fusedEulerPose[VEC3_Z] * RAD_TO_DEGREE;
		}
	}
}
#endif
