#include "SysConfig.h"
#if(HAS_BNO055)

#include "CBNO055.h"
#include "NDataManager.h"
#include "NCommManager.h"

using namespace bno055;

namespace
{
	constexpr uint32_t kMaxHardResets 		= 5;
	
	constexpr uint32_t kStatusCheckDelay_ms	= 1000;							// 1hz
	constexpr uint32_t kTelemetryDelay_ms 	= 50;							// 20hz
	constexpr uint32_t kCalibDelay_ms 		= bno055::kCalUpdateRate_ms;	// 1hz
}

CBNO055::CBNO055( I2C *i2cInterfaceIn, bno055::EAddress addressIn )
	: m_device( i2cInterfaceIn, addressIn )
{
}

void CBNO055::Initialize()
{
	Serial.println( F( "bno055_init:1;" ) );
	
	m_statusCheckTimer.Reset();
	m_telemetryTimer.Reset();
	m_calibTimer.Reset();

	// 25% max failure rate before 
	m_maxFailuresPerPeriod = ( kStatusCheckDelay_ms / 4 ) / m_device.GetUpdatePeriod();
}

void CBNO055::Update( CCommand &commandIn )
{
	if( m_device.IsEnabled() == false )
	{
		return;
	}

	// Handle commands
	if( NCommManager::m_isCommandAvailable )
	{
		// Zero the yaw value
		if( commandIn.Equals( "imu_zyaw" ) )
		{
			// Set offset based on current value
			m_yawOffset = m_device.m_data.yaw;

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
			// For now: 0-GYRO, 1-COMPASS
			if( commandIn.m_arguments[1] == 0 )
			{
				m_device.SetMode( EMode::GYRO );

				Serial.print( F( "imu_mode:" ) );
				Serial.print( commandIn.m_arguments[1] );
				Serial.println( ';' );
			}
			else if( commandIn.m_arguments[1] == 1 )
			{
				m_device.SetMode( EMode::COMPASS );

				Serial.print( F( "imu_mode:" ) );
				Serial.print( commandIn.m_arguments[1] );
				Serial.println( ';' );
			}
		}
	}

	// Handle health checks
	if( m_statusCheckTimer.HasElapsed( kStatusCheckDelay_ms ) )
	{
		// Check to see if the error threshold is above acceptable levels
		if( m_device.GetResultCount( EResult::RESULT_ERR_READING_EULER ) > m_maxFailuresPerPeriod )
		{
			Serial.println( "bno055_HardReset:1" );
			m_device.HardReset();
		}
		else
		{
			// Clear the error counter
			m_device.ClearResultCount( EResult::RESULT_ERR_READING_EULER );
		}
		
		// Check to see if we have surpassed our hard reset threshhold 
		if( m_device.GetResultCount( EResult::RESULT_ERR_HARD_RESET ) > kMaxHardResets )
		{
			// Permanently disable the device
			m_device.Disable();
			Serial.println( F( "bno055_disabled:1;" ) );
			return;
		}
	}

	// Run the state machine
	m_device.Tick();

	// Update shared navdata 
	if( m_device.m_data.SampleAvailable() )
	{	
		if( m_device.GetMode() == EMode::GYRO )
		{
			NDataManager::m_navData.YAW	= NORMALIZE_ANGLE_180( m_device.m_data.yaw - m_yawOffset );
		}
		else
		{
			NDataManager::m_navData.YAW	= NORMALIZE_ANGLE_360( m_device.m_data.yaw );
		}
		
		NDataManager::m_navData.PITC	= m_device.m_data.pitch - m_pitchOffset;
		NDataManager::m_navData.ROLL	= m_device.m_data.roll - m_rollOffset;

		// Emit telemetry
		if( m_telemetryTimer.HasElapsed( kTelemetryDelay_ms ) )
		{
			Serial.print( F( "imu_r:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.ROLL ) ); 	Serial.print( ';' );
			Serial.print( F( "imu_p:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.PITC ) ); 	Serial.print( ';' );
			Serial.print( F( "imu_y:" ) );	Serial.print( orutil::Encode1K( NDataManager::m_navData.YAW ) ); 	Serial.println( ';' );
		}
	}

	// Report calibration levels of each subsystem
	if( m_device.m_calibration.SampleAvailable() )
	{	
		if( m_calibTimer.HasElapsed( kCalibDelay_ms ) )
		{
			Serial.print( F( "bno055_cAcc:" ) );	Serial.print( m_device.m_calibration.accel ); 	Serial.print( ';' );
			Serial.print( F( "bno055_cGyr:" ) );	Serial.print( m_device.m_calibration.gyro ); 	Serial.print( ';' );
			Serial.print( F( "bno055_cMag:" ) );	Serial.print( m_device.m_calibration.mag ); 	Serial.print( ';' );
			Serial.print( F( "bno055_cSys:" ) );	Serial.print( m_device.m_calibration.system ); 	Serial.println( ';' );
		}
	}
}

#endif