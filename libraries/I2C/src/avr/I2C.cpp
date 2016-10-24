#if( ARDUINO_ARCH_AVR )

#include <I2C.h>
#include "I2C_Driver.h"

using namespace i2c;

#define ENABLE_I2C_DEBUG 1

// Pass in this option to enable error counting
#ifdef ENABLE_I2C_DEBUG
	#define HANDLE_RESULT( result ) ( m_statistics.AddResult( result ) )
#else
	#define HANDLE_RESULT( result ) ( result )
#endif

I2C::I2C()
{
}

EI2CResult I2C::Enable()
{
    if( m_isEnabled )
	{
		// Already enabled
		return HANDLE_RESULT( EI2CResult::RESULT_ERR_ALREADY_INITIALIZED );
	}

	// Enable the interface
	m_result = i2c::avr::Enable( m_baudRate );

	if( m_result )
	{
		// Failed to enable
		return HANDLE_RESULT( m_result );
	}
	
    // Mark as enabled
	m_isEnabled = true;

	// Success
	return HANDLE_RESULT( m_result );
}

EI2CResult I2C::Disable()
{
	// Disable the interface
    i2c::avr::Disable();
    m_isEnabled = false;

    return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

EI2CResult I2C::Reset()
{
	i2c::avr::Reset();

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

bool I2C::LockAddress( uint8_t addressIn )
{
	// Validate address
	if( addressIn >= 127 )
	{
		return false;
	}

	if( m_addressLocks[ addressIn ] == 1 )
	{
		// Address already locked
		return false;
	}
	else
	{
		// Lock address
		m_addressLocks[ addressIn ] = 1;
		return true;
	}
}

void I2C::FreeAddress( uint8_t addressIn )
{
	// Validate address
	if( addressIn >= 127 )
	{
		return;
	}

	// Free the lock
	m_addressLocks[ addressIn ] = 0;
}

bool I2C::IsAvailable()
{
    return m_isEnabled;
}

EI2CResult I2C::SetBaudRate( EI2CBaudRate baudRateIn )
{
	m_result = i2c::avr::SetBaudRate( baudRateIn );

	if( m_result )
	{
		// Failed
		return HANDLE_RESULT( m_result );
	}

	// Update baud rate
	m_baudRate = baudRateIn;

	// Success
	return HANDLE_RESULT( m_result );
}

// -----------------------
// Transaction Methods
// -----------------------

EI2CResult I2C::WriteByte( uint8_t slaveAddressIn, uint8_t dataIn, bool issueRepeatedStart )
{
	// Start transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write byte
	m_result = i2c::avr::WriteByte( dataIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

EI2CResult I2C::WriteRegisterByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t dataIn, bool issueRepeatedStart )
{
	// Start transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write Register Address
	m_result = i2c::avr::WriteByte( registerIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write Data byte
	m_result = i2c::avr::WriteByte( dataIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

EI2CResult I2C::WriteBytes( uint8_t slaveAddressIn, uint8_t *dataIn, uint8_t numberBytesIn,  bool issueRepeatedStart )
{
	// Start transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write multiple bytes
	for( size_t i = 0; i < numberBytesIn; ++i )
	{
		m_result = i2c::avr::WriteByte( dataIn[ i ] );
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

EI2CResult I2C::WriteRegisterBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataIn, uint8_t numberBytesIn, bool issueRepeatedStart )
{
	// Start transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write Register Address
	m_result = i2c::avr::WriteByte( registerIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write multiple bytes
	for( size_t i = 0; i < numberBytesIn; ++i )
	{
		m_result = i2c::avr::WriteByte( dataIn[ i ] );
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

// Read operations
EI2CResult I2C::ReadRegisterByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, bool issueRepeatedStart )
{
	// Start transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write Register Address
	m_result = i2c::avr::WriteByte( registerIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Start read transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_READ );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Read Byte and send nack
	m_result = i2c::avr::ReadByte( dataOut, true );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

EI2CResult I2C::ReadRegisterBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, uint8_t numberBytesIn, bool issueRepeatedStart )
{
	// Start write transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_WRITE );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Write Register Address
	m_result = i2c::avr::WriteByte( registerIn );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Start read transaction
	m_result = i2c::avr::StartTransaction();
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Send address
	m_result = i2c::avr::SendAddress( slaveAddressIn, EI2CAction::I2C_READ );
	if( m_result ){ return HANDLE_RESULT( m_result ); }

	// Read in the requested amount of bytes
	for( size_t i = 0; i < numberBytesIn; ++i )
	{
		if( i == ( numberBytesIn - 1 ) )
		{
			// Last byte, send NACK
			m_result = i2c::avr::ReadByte( dataOut + i, true );
			if( m_result ){ return HANDLE_RESULT( m_result ); }
		}
		else
		{
			// Not the last byte, send ACK
			m_result = i2c::avr::ReadByte( dataOut + i, false );
			if( m_result ){ return HANDLE_RESULT( m_result ); }
		}
	}

	// Send stop, if requested
	if( !issueRepeatedStart )
	{
		m_result = i2c::avr::StopTransaction();
		if( m_result ){ return HANDLE_RESULT( m_result ); }
	}

	return HANDLE_RESULT( EI2CResult::RESULT_SUCCESS );
}

// -------------------------------------------------------
// Instantiation - Only one I2C interface on the AVR chips

I2C I2C0;

#endif