// Includes
#include "SysConfig.h"

#if( HAS_MPU9150 )

#include "CMPU9150.h"

#include "NDataManager.h"
#include "NCommManager.h"
#include "CModule.h"
#include <orutil.h>

#define MPU_MODULE_ID 0

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

//  MPU_LPF_RATE is the low pass filter rate and can be between 5 and 188Hz
#define MPU_LPF_RATE   40

using namespace mpu9150;

namespace
{
	constexpr uint32_t kMaxHardResets 		= 5;
	
	constexpr uint32_t kStatusCheckDelay_ms	= 1000;							// 1hz
	constexpr uint32_t kTelemetryDelay_ms 	= 50;							// 20hz
	constexpr uint32_t kResetDelay_ms		= 1000;
	
	// Attempt to read at twice rate of updates to prevent MPU FIFO overflow 
	// Normally handled by using a physical interrupt to service on demand
	constexpr uint32_t kDataUpdateRate_ms	= ( ( 1000 / MPU_UPDATE_RATE ) / 2 );	
}

CMPU9150::CMPU9150( mpu9150::EAddress addressIn )
	: m_device( addressIn )
{
}

void CMPU9150::Initialize()
{
	Serial.println( F( "mpu9150_init:0;" ) );
	
	m_statusCheckTimer.Reset();
	m_telemetryTimer.Reset();
	m_updateTimer.Reset();
	m_resetTimer.Reset();

	// 25% max failure rate before 
	m_maxFailuresPerPeriod = 30;
}

void CMPU9150::Update( CCommand& commandIn )
{
	// If disabled, do nothing
	if( m_isDisabled )
	{
		return;
	}

	// Handle initialization if necessary
	if( m_isInitialized == false )
	{
		if( m_device.GetResultCount( EResult::RESULT_ERR_HARD_RESET ) > kMaxHardResets )
		{
			m_isDisabled = true;
			Serial.println( F( "mpu9150_disabled:1;" ) );
			return;
		}
		else
		{
			if( m_resetTimer.HasElapsed( kResetDelay_ms ) )
			{
				// Attempt to initialize
				if( m_device.init( MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE ) == true )
				{
					// Success
					m_isInitialized = true;
					Serial.println( F( "mpu9150_init:1;" ) );
				}
				else
				{
					Serial.println( F( "mpu9150_init:0;" ) );
				}

				// Increment hard reset counter, whether init succeeded or not
				m_device.AddResult( EResult::RESULT_ERR_HARD_RESET );

				return;
			}
			else
			{
				return;
			}
		}
	}

	// Handle commands
	if( NCommManager::m_isCommandAvailable )
	{
		// Zero the yaw value
		if( commandIn.Equals( "imu_zyaw" ) )
		{
			// Set offset based on current value
			m_yawOffset = NDataManager::m_navData.YAW;

			// Send ack
			Serial.println( F( "imu_zyaw:ack;" ) );
		}
		// Zero the roll and pich values
		else if( commandIn.Equals( "imu_level" ) )
		{
			// Set offsets based on request from cockpit
			m_rollOffset 	= orutil::Decode1K( commandIn.m_arguments[1] );
			m_pitchOffset 	= orutil::Decode1K( commandIn.m_arguments[2] );

			// Report new settings
			Serial.print( F( "imu_roff:" ) ); Serial.print( commandIn.m_arguments[1] ); Serial.println( ';' );
			Serial.print( F( "imu_poff:" ) ); Serial.print( commandIn.m_arguments[2] ); Serial.println( ';' );
		}
		// Set the operating mode
		else if( commandIn.Equals( "imu_mode" ) )
		{
			// Does not support mode changes right now, send nack
			Serial.println( F( "imu_level:nack;" ) );
		}
	}

	// Handle health checks
	if( m_statusCheckTimer.HasElapsed( kStatusCheckDelay_ms ) )
	{
		// Check to see if the error threshold is above acceptable levels
		if( m_device.GetResultCount( EResult::RESULT_ERR_READ_ERROR ) > m_maxFailuresPerPeriod )
		{
			// Trigger a hard reset on the next loop
			m_isInitialized = false;

			// End this run of Update()
			return;
		}
		else
		{
			// Clear the error counter
			m_device.ClearResultCount( EResult::RESULT_ERR_READ_ERROR );
		}
	}

	// Read data
	if( m_updateTimer.HasElapsed( kDataUpdateRate_ms ) )
	{
		if( m_device.read() == true )
		{
			// Successfully got data, update shared state
			NDataManager::m_navData.ROLL 	= m_device.m_fusedEulerPose[VEC3_X] * RAD_TO_DEGREE - m_rollOffset;
			NDataManager::m_navData.PITC 	= m_device.m_fusedEulerPose[VEC3_Y] * RAD_TO_DEGREE - m_pitchOffset;
			NDataManager::m_navData.YAW 	= NORMALIZE_ANGLE_180( ( m_device.m_fusedEulerPose[VEC3_Z] * RAD_TO_DEGREE ) - m_yawOffset );

			// Handle telemetry updates
			if( m_telemetryTimer.HasElapsed( kTelemetryDelay_ms ) )
			{
				Serial.print( F( "imu_r:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.ROLL ) ); 	Serial.print( ';' );
				Serial.print( F( "imu_p:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.PITC ) ); 	Serial.print( ';' );
				Serial.print( F( "imu_y:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.YAW ) ); 	Serial.println( ';' );
			}
		}
	}
}
#endif
