/***************************************************************************
  This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/

// Includes
#include "Arduino.h"
#include <math.h>
#include <limits.h>

#include "CI2C.h"
#include "CAdaBNO055.h"

CAdaBNO055::CAdaBNO055( int32_t sensorIdIn, uint8_t addressIn )
{
	m_isInitialized = false;
	m_lastRetcode	= true;
	m_sensorId		= sensorIdIn;
	m_i2cAddress	= addressIn;
}

bool CAdaBNO055::Initialize()
{
	m_isInitialized = false;

	delay( 500 );

	// Idempotent
	// Verify chip ID
	if( VerifyChipId() != true )
	{
		return false;
	}

	delay( 10 );

	// Non-idempotent - Uninitializes and puts device in unconfigured default state
	// Reset the device
	if( Reset() != true )
	{
		return false;
	}

	unsigned long start = millis();

	// Idempotent
	// Try every 10ms to verify the chip ID again after reset
	while( true )
	{
		m_lastRetcode = VerifyChipId();

		delay( 10 );

		if( m_lastRetcode == true )
		{
			// Successfully reset
			break;
		}

		if( ( millis() - start ) > 1000 )
		{
			// TODO: Error message
			return m_lastRetcode;

		}
	}

	// Idempotent
	// Switch to config mode
	if( WriteByte( BNO055_OPR_MODE_ADDR, ( uint8_t )OPERATION_MODE_CONFIG ) != true )
	{
		return false;
	}

	delay( 1000 );

	if( SetSelectedRegisterPage( 0 ) != true )
	{
		return false;
	}

	delay( 10 );

	// Get the Power-on self test results
	if( GetPowerOnSelfTestResults() != true )
	{
		return false;
	}

	delay( 10 );

	if( m_powerOnSelfTestResult != 0x0F )
	{
		return false;
	}

	delay( 10 );

	if( GetSoftwareVersion() != true )
	{
		return false;
	}

	delay( 10 );

	if( GetBootloaderRev() != true )
	{
		return false;
	}

	// Idempotent
	if( SetUpUnitsAndOrientation() != true )
	{
		return false;
	}

	delay( 10 );

	// Idempotent
	// Set the power mode to normal
	if( SetPowerMode( POWER_MODE_NORMAL ) != true )
	{
		return false;
	}

	delay( 10 );

	// Move to NDOF mode out of config mode
	if( WriteByte( BNO055_OPR_MODE_ADDR, ( uint8_t )OPERATION_MODE_NDOF ) != true )
	{
		return false;
	}

	delay( 100 );

	m_isInitialized = true;

	return true;
}

bool CAdaBNO055::SetMode( EOpMode modeIn )
{
	if( !WriteByte( BNO055_OPR_MODE_ADDR, modeIn ) )
	{
		return false;
	}

	// Update our current mode
	m_mode = modeIn;

	// TODO: Make this smart about which delay time to use. For now, default to 600
	// Wait for required amount of time for mode change
	delay( k_configToOpDelay_ms );

	return true;
}

bool CAdaBNO055::SetExternalCrystalUse( boolean shouldUseIn )
{
	EOpMode originalMode = m_mode;

	// Switch to config mode (just in case since this is the default)
	if( SetMode( OPERATION_MODE_CONFIG ) != true )
	{
		return false;
	}

	// Make sure we are on page 0
	if( !WriteByte( BNO055_PAGE_ID_ADDR, 0 ) )
	{
		return false;
	}

	// Set the external crystal use register
	if( shouldUseIn )
	{
		if( !WriteByte( BNO055_SYS_TRIGGER_ADDR, 0x80 ) )
		{
			return false;
		}
	}
	else
	{
		if( !WriteByte( BNO055_SYS_TRIGGER_ADDR, 0x00 ) )
		{
			return false;
		}
	}

	// Go back to the mode we were originally in
	if( SetMode( originalMode )  != true )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetSystemStatus()
{
	// System Status (see section 4.3.58)
	// ---------------------------------
	// 0 = Idle
	// 1 = System Error
	// 2 = Initializing Peripherals
	// 3 = System Iniitalization
	// 4 = Executing Self-Test
	// 5 = Sensor fusio algorithm running
	// 6 = System running without fusion algorithms

	if( !ReadByte( BNO055_SYS_STAT_ADDR, m_systemStatus ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetSystemError()
{
	// System Error (see section 4.3.59)
	// ---------------------------------
	// 0 = No error
	// 1 = Peripheral initialization error
	// 2 = System initialization error
	// 3 = Self test result failed
	// 4 = Register map value out of range
	// 5 = Register map address out of range
	// 6 = Register map write error
	// 7 = BNO low power mode not available for selected operat ion mode
	// 8 = Accelerometer power mode not available
	// 9 = Fusion algorithm configuration error
	// A = Sensor configuration error

	if( !ReadByte( BNO055_SYS_ERR_ADDR, m_systemError ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetSoftwareVersion()
{
	if( !ReadByte( BNO055_SW_REV_ID_LSB_ADDR, m_softwareVersionMinor ) )
	{
		return false;
	}

	if( !ReadByte( BNO055_SW_REV_ID_MSB_ADDR, m_softwareVersionMajor ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetBootloaderRev()
{
	if( !ReadByte( BNO055_BL_REV_ID_ADDR, m_bootloaderRev ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetOperatingMode()
{
	if( !ReadByte( BNO055_OPR_MODE_ADDR, m_operatingMode ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetAccelerometerOffsets()
{
	bool ret = true;

	uint8_t lsb = 0;
	uint8_t msb = 0;

	ret &= ReadByte( ACCEL_OFFSET_X_LSB_ADDR, lsb );
	ret &= ReadByte( ACCEL_OFFSET_X_MSB_ADDR, msb );

	m_accOffsets.x = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( ACCEL_OFFSET_Y_LSB_ADDR, lsb );
	ret &= ReadByte( ACCEL_OFFSET_Y_MSB_ADDR, msb );

	m_accOffsets.y = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( ACCEL_OFFSET_Z_LSB_ADDR, lsb );
	ret &= ReadByte( ACCEL_OFFSET_Z_MSB_ADDR, msb );

	m_accOffsets.z = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	if( ret )
	{
		Serial.print( "BNO055.acc.offset.x:" );
		Serial.print( m_accOffsets.x );
		Serial.println( ";" );

		Serial.print( "BNO055.acc.offset.y:" );
		Serial.print( m_accOffsets.y );
		Serial.println( ";" );

		Serial.print( "BNO055.acc.offset.z:" );
		Serial.print( m_accOffsets.z );
		Serial.println( ";" );
	}

	return ret;
}

bool CAdaBNO055::GetGyroOffsets()
{
	bool ret = true;

	uint8_t lsb = 0;
	uint8_t msb = 0;

	ret &= ReadByte( GYRO_OFFSET_X_LSB_ADDR, lsb );
	ret &= ReadByte( GYRO_OFFSET_X_MSB_ADDR, msb );

	m_gyroOffsets.x = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( GYRO_OFFSET_Y_LSB_ADDR, lsb );
	ret &= ReadByte( GYRO_OFFSET_Y_MSB_ADDR, msb );

	m_gyroOffsets.y = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( GYRO_OFFSET_Z_LSB_ADDR, lsb );
	ret &= ReadByte( GYRO_OFFSET_Z_MSB_ADDR, msb );

	m_gyroOffsets.z = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	if( ret )
	{
		Serial.print( "BNO055.gyro.offset.x:" );
		Serial.print( m_gyroOffsets.x );
		Serial.println( ";" );

		Serial.print( "BNO055.gyro.offset.y:" );
		Serial.print( m_gyroOffsets.y );
		Serial.println( ";" );

		Serial.print( "BNO055.gyro.offset.z:" );
		Serial.print( m_gyroOffsets.z );
		Serial.println( ";" );
	}

	return ret;
}

bool CAdaBNO055::GetMagnetometerOffsets()
{
	bool ret = true;

	uint8_t lsb = 0;
	uint8_t msb = 0;

	ret &= ReadByte( MAG_OFFSET_X_LSB_ADDR, lsb );
	ret &= ReadByte( MAG_OFFSET_X_MSB_ADDR, msb );

	m_magOffsets.x = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( MAG_OFFSET_Y_LSB_ADDR, lsb );
	ret &= ReadByte( MAG_OFFSET_Y_MSB_ADDR, msb );

	m_magOffsets.y = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	ret &= ReadByte( MAG_OFFSET_Z_LSB_ADDR, lsb );
	ret &= ReadByte( MAG_OFFSET_Z_MSB_ADDR, msb );

	m_magOffsets.z = ( ( ( uint16_t )msb ) << 8 ) | ( ( uint16_t )lsb );

	if( ret )
	{
		Serial.print( "BNO055.mag.offset.x:" );
		Serial.print( m_magOffsets.x );
		Serial.println( ";" );

		Serial.print( "BNO055.mag.offset.y:" );
		Serial.print( m_magOffsets.y );
		Serial.println( ";" );

		Serial.print( "BNO055.mag.offset.z:" );
		Serial.print( m_magOffsets.z );
		Serial.println( ";" );
	}

	return ret;
}

bool CAdaBNO055::EnterIMUMode()
{
	// Get all of the offsets
	GetGyroOffsets();
	GetAccelerometerOffsets();
	GetMagnetometerOffsets();

	//Serial.print( "BNO055.gyro.offsets:" );
	//Serial.print( m_gyroOffsets.x );
	//Serial.print( "|" );
	//Serial.print( m_gyroOffsets.y );
	//Serial.print( "|" );
	//Serial.print( m_gyroOffsets.z );
	//Serial.println( ";" );

	//Serial.print( "BNO055.acc.offsets:" );
	//Serial.print( m_accOffsets.x );
	//Serial.print( "|" );
	//Serial.print( m_accOffsets.y );
	//Serial.print( "|" );
	//Serial.print( m_accOffsets.z );
	//Serial.println( ";" );

	//Serial.print( "BNO055.mag.offsets:" );
	//Serial.print( m_magOffsets.x );
	//Serial.print( "|" );
	//Serial.print( m_magOffsets.y );
	//Serial.print( "|" );
	//Serial.print( m_magOffsets.z );
	//Serial.println( ";" );

	// Switch to config mode first
	if( SetMode( OPERATION_MODE_CONFIG ) != true )
	{
		return false;
	}

	// Switch to IMU mode
	if( SetMode( OPERATION_MODE_IMUPLUS ) != true )
	{
		return false;
	}

	Serial.print( "bno.mode:" );
	Serial.print( "IMU" );
	Serial.println( ';' );
}

bool CAdaBNO055::EnterNDOFMode()
{
	// Get all of the offsets
	GetGyroOffsets();
	GetAccelerometerOffsets();
	GetMagnetometerOffsets();

	// Switch to config mode first
	if( SetMode( OPERATION_MODE_CONFIG ) != true )
	{
		return false;
	}

	// Switch to NDOF mode
	if( SetMode( OPERATION_MODE_NDOF ) != true )
	{
		return false;
	}

	Serial.print( "bno.mode:" );
	Serial.print( "NDOF" );
	Serial.println( ';' );
}

bool CAdaBNO055::GetRevInfo( TRevisionInfo& revInfoOut )
{
	uint8_t a, b;

	// Clear the rev info
	revInfoOut.accel_rev = 0;
	revInfoOut.mag_rev = 0;
	revInfoOut.gyro_rev = 0;
	revInfoOut.bl_rev = 0;
	revInfoOut.sw_rev = 0;

	bool ret = true;

	ret &= ReadByte( BNO055_ACCEL_REV_ID_ADDR, revInfoOut.accel_rev );	// Accelerometer
	ret &= ReadByte( BNO055_MAG_REV_ID_ADDR, revInfoOut.mag_rev );		// Magnetometer
	ret &= ReadByte( BNO055_GYRO_REV_ID_ADDR, revInfoOut.gyro_rev );		// Gyrometer
	ret &= ReadByte( BNO055_BL_REV_ID_ADDR, revInfoOut.bl_rev );			// Bootloader
	ret &= ReadByte( BNO055_SW_REV_ID_LSB_ADDR, a );						// Software Version LSB
	ret &= ReadByte( BNO055_SW_REV_ID_MSB_ADDR, b );						// Software Version MSB

	if( !ret )
	{
		return false;
	}

	// Combine the LSB and MSB
	revInfoOut.sw_rev = ( ( ( uint16_t )b ) << 8 ) | ( ( uint16_t )a );

	return true;
}

bool CAdaBNO055::GetCalibration()
{
	uint8_t calData;

	if( !ReadByte( BNO055_CALIB_STAT_ADDR, calData ) )
	{
		return false;
	}

	m_systemCal	= ( calData >> 6 ) & 0x03;
	m_gyroCal	= ( calData >> 4 ) & 0x03;
	m_accelCal	= ( calData >> 2 ) & 0x03;
	m_magCal	= calData & 0x03;

	return true;
}

bool CAdaBNO055::VerifyChipId()
{
	// Read the chip ID
	uint8_t id;

	if( ReadByte( BNO055_CHIP_ID_ADDR, id ) != true )
	{
		return false;
	}

	if( id != BNO055_ID )
	{
		return false;
	}

	// Check to see if it matches the proper ID (0xA0)
	return true;
}

bool CAdaBNO055::Reset()
{
	// Set System Reset bit
	if( !WriteByte( BNO055_SYS_TRIGGER_ADDR, 0x20 ) )
	{
		return false;
	}

	// Delay for required time to perform a reset
	delay( k_resetDelay_ms );

	return true;
}

bool CAdaBNO055::SetPowerMode( EPowerMode powerModeIn )
{
	if( !WriteByte( BNO055_PWR_MODE_ADDR, powerModeIn ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::SetSelectedRegisterPage( byte pageIdIn )
{
	// TODO: Bounding checks
	if( !WriteByte( BNO055_PAGE_ID_ADDR, pageIdIn ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::SetUpUnitsAndOrientation()
{
	// Select the output units
	uint8_t unitSelect =	( 0 << 7 ) |	// Orientation = Android
	                        ( 0 << 4 ) |	// Temperature = Celsius
	                        ( 0 << 2 ) |	// Euler = Degrees
	                        ( 0 << 1 ) |	// Gyro = Deg/s
	                        ( 1 << 0 );		// Accelerometer = mg


	bool ret = true;

	ret &= WriteByte( BNO055_UNIT_SEL_ADDR, unitSelect );

	// Wires forward, sensors down
	// What this actually means is: X=X, Y=-Y, Z=-Z
	ret &= WriteByte( BNO055_AXIS_MAP_CONFIG_ADDR, 0x24 );	// This is the default value
	ret &= WriteByte( BNO055_AXIS_MAP_SIGN_ADDR, 0x03 );		// Negate Y and Z axes from default (Z-down configuration)

	if( !ret )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetTemperature( int8_t& temperatureOut )
{
	uint8_t temp;

	if( !ReadByte( BNO055_TEMP_ADDR, temp ) )
	{
		return false;
	}

	temperatureOut = ( int8_t )temp;

	return true;
}

bool CAdaBNO055::GetPowerOnSelfTestResults()
{
	// Self Test Results (see section )
	// --------------------------------
	// 1 = test passed, 0 = test failed
	//
	// Bit 0 = Accelerometer self test
	// Bit 1 = Magnetometer self test
	// Bit 2 = Gyroscope self test
	// Bit 3 = MCU self test
	//
	// 0x0F = all good!

	if( !ReadByte( BNO055_SELFTEST_RESULT_ADDR, m_powerOnSelfTestResult ) )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::GetVector( EVectorType vectorTypeIn, imu::Vector<3>& vectorOut )
{
	// Create and zero buffer
	uint8_t buffer[6];
	memset( buffer, 0, 6 );

	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;


	// Read vector data (6 bytes)
	//if( !ReadNBytes( ( ERegisterAddress )vectorTypeIn, buffer, 6 ) )
	//{
	//return false;
	//}

	if( I2c.read( m_i2cAddress, ( uint8_t )vectorTypeIn, ( uint8_t )6 ) )
	{
		return false;
	}

	buffer[ 0 ] = I2c.receive();
	buffer[ 1 ] = I2c.receive();
	buffer[ 2 ] = I2c.receive();
	buffer[ 3 ] = I2c.receive();
	buffer[ 4 ] = I2c.receive();
	buffer[ 5 ] = I2c.receive();


	x = ( ( int16_t )buffer[0] ) | ( ( ( int16_t )buffer[1] ) << 8 );
	y = ( ( int16_t )buffer[2] ) | ( ( ( int16_t )buffer[3] ) << 8 );
	z = ( ( int16_t )buffer[4] ) | ( ( ( int16_t )buffer[5] ) << 8 );

	// Convert the value to an appropriate range and assign the value to the Vector type
	switch( vectorTypeIn )
	{
		case VECTOR_MAGNETOMETER:
			// 1uT = 16 LSB
			vectorOut[0] = ( ( double )x ) / 16.0;
			vectorOut[1] = ( ( double )y ) / 16.0;
			vectorOut[2] = ( ( double )z ) / 16.0;
			break;

		case VECTOR_GYROSCOPE:
			// 1rps = 900 LSB
			vectorOut[0] = ( ( double )x ) / 900.0;
			vectorOut[1] = ( ( double )y ) / 900.0;
			vectorOut[2] = ( ( double )z ) / 900.0;
			break;

		case VECTOR_EULER:
			// 1 degree = 16 LSB
			vectorOut[0] = ( ( double )x ) / 16.0;
			vectorOut[1] = ( ( double )y ) / 16.0;
			vectorOut[2] = ( ( double )z ) / 16.0;
			break;

		case VECTOR_ACCELEROMETER:
		case VECTOR_LINEARACCEL:
		case VECTOR_GRAVITY:
			// 1m/s^2 = 100 LSB
			vectorOut[0] = ( ( double )x ) / 100.0;
			vectorOut[1] = ( ( double )y ) / 100.0;
			vectorOut[2] = ( ( double )z ) / 100.0;
			break;
	}

	return true;
}

bool CAdaBNO055::GetQuat( imu::Quaternion& quatOut )
{
	// Create and zero buffer
	uint8_t buffer[8];
	memset( buffer, 0, 8 );

	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	int16_t w = 0;


	// Read quat data (8 bytes)
	if( !ReadNBytes( BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8 ) )
	{
		return false;
	}

	w = ( ( ( uint16_t )buffer[1] ) << 8 ) | ( ( uint16_t )buffer[0] );
	x = ( ( ( uint16_t )buffer[3] ) << 8 ) | ( ( uint16_t )buffer[2] );
	y = ( ( ( uint16_t )buffer[5] ) << 8 ) | ( ( uint16_t )buffer[4] );
	z = ( ( ( uint16_t )buffer[7] ) << 8 ) | ( ( uint16_t )buffer[6] );

	// Assign to quaternion
	// See http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_12~1.pdf 3.6.5.5 Orientation (Quaternion)
	const double scale = ( 1.0 / ( 1 << 14 ) );

	quatOut = imu::Quaternion( scale * w, scale * x, scale * y, scale * z );

	return true;
}



/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

bool CAdaBNO055::WriteByte( ERegisterAddress addressIn, uint8_t dataIn )
{
	uint8_t ret = I2c.write( m_i2cAddress, ( uint8_t )addressIn, dataIn );

	// Non-zero is failure
	if( ret )
	{
		return false;
	}

	return true;
}

bool CAdaBNO055::ReadByte( ERegisterAddress addressIn, uint8_t& dataOut )
{

	// Set address to request from
	uint8_t ret = I2c.read( m_i2cAddress, ( uint8_t )addressIn, ( uint8_t )1 );

	// Non-zero failure
	if( ret )
	{
		return false;
	}

	// Request single byte from slave
	dataOut = I2c.receive();

	return true;
}

bool CAdaBNO055::ReadNBytes( ERegisterAddress addressIn, uint8_t* dataOut, uint8_t byteCountIn )
{
	// Set address to request from
	uint8_t ret = I2c.read( m_i2cAddress, ( uint8_t )addressIn, byteCountIn, dataOut );

	// Non-zero failure
	if( ret )
	{
		return false;
	}

	return true;
}
