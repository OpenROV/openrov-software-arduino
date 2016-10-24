// Includes
#include "MS5837_30BA.h"
#include <assert.h>

using namespace ms5837_30ba;

MS5837_30BA::MS5837_30BA( I2C *i2cInterfaceIn, EAddress addressIn )
    : m_pI2C( i2cInterfaceIn )
    , m_address( ( addressIn == EAddress::ADDRESS_A ? I2C_ADDRESS_A : I2C_ADDRESS_B ) )
{
}

// --------------------------------------------------------------
// Public API
// --------------------------------------------------------------

void MS5837_30BA::Tick()
{
    // TODO: Clever way to do this with function pointers and lambdas?
    switch( m_state )
    {
        case EState::DELAY:
        {
            // Do nothing until requested time has elapsed
            if( m_delay.timer.HasElapsed( m_delay.delayTime_ms ) )
            {
                // Move to next state
                Transition( m_delay.nextState );
            }

            break;
        }

        // -----------------------------
        // Active operating conditions
        // -----------------------------
        case EState::CONVERTING_PRESSURE:
        {
            // Start a pressure conversion
            if( Cmd_StartPresConversion() )
            {
                // Failure, start new conversion sequence
                DelayedTransition( EState::CONVERTING_PRESSURE, kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms );
                m_results.AddResult( EResult::RESULT_ERR_FAILED_SEQUENCE );
            }
            else
            {
                // Success, next read the value and start converting pressure
                DelayedTransition( EState::CONVERTING_TEMPERATURE, kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms );
            }

            break;
        };

        case EState::CONVERTING_TEMPERATURE:
        {
            // Read pressure
            if( Cmd_ReadPressure() )
            {
                // Failure, start new conversion sequence
                DelayedTransition( EState::CONVERTING_PRESSURE, kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms );
                m_results.AddResult( EResult::RESULT_ERR_FAILED_SEQUENCE );
            }
            else
            {
                // Start temp conversion
                if( Cmd_StartTempConversion() )
                {
                    // Failure, start new conversion sequence
                    DelayedTransition( EState::CONVERTING_PRESSURE, kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms );
                    m_results.AddResult( EResult::RESULT_ERR_FAILED_SEQUENCE );
                }
                else
                {
                    // Success
                    DelayedTransition( EState::PROCESSING_DATA, kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms );
                }
            }

            break;
        };

        case EState::PROCESSING_DATA:
        {
            // Read temperature
            if( Cmd_ReadTemperature() )
            {
                // Failure, start new conversion sequence
                Transition( EState::CONVERTING_PRESSURE );
                m_results.AddResult( EResult::RESULT_ERR_FAILED_SEQUENCE );
            }
            else
            {
                // Calculate depth and announce available data sample
                ProcessData();

                // Return to for the next read
                Transition( EState::CONVERTING_PRESSURE );
            }

            break;
        };

        // ------------------------
        // Initialization routines
        // ------------------------

        case EState::UNINITIALIZED:
        {
            // Attempt to reset
            if( Cmd_Reset() )
            {
                // Failure
                HardReset();
            }
            else
            {
                // Sucess
                DelayedTransition( EState::READING_CALIB_DATA, kResetDelay_ms );
            }

            break;
        }

        case EState::READING_CALIB_DATA:
        {
            // Fetch calibration coefficients
            if( Cmd_ReadCalibrationData() )
            {
                // Failure
                HardReset();
            }
            else
            {
                // Success
                Transition( EState::CONVERTING_PRESSURE );
            }

            break;
        };

        case EState::DISABLED:
        default:
        {
            // Do nothing
            break;
        }
    }
}

void MS5837_30BA::HardReset()
{
    // Perform delayed transition to uninitialized
    DelayedTransition( EState::UNINITIALIZED, kRetryDelay_ms );

    // Increment hard reset counter
    m_results.AddResult( EResult::RESULT_ERR_HARD_RESET );

    // Clear the sequence error counter
    ClearResultCount( EResult::RESULT_ERR_FAILED_SEQUENCE );
}

void MS5837_30BA::FullReset()
{
    // Perform delayed transition to uninitialized
    DelayedTransition( EState::UNINITIALIZED, kRetryDelay_ms );

    // Clear all results
    m_results.Clear();

    // Re-enable device
    m_enabled = true;
}

uint32_t MS5837_30BA::GetResultCount( EResult resultTypeIn )
{
    return m_results.GetResultCount( resultTypeIn );
}

void MS5837_30BA::ClearResultCount( EResult resultTypeIn )
{
    m_results.ClearResult( resultTypeIn );
}

void MS5837_30BA::Disable()
{
    // Disable device entirely
    Transition( EState::DISABLED );

    m_enabled = false;
}

bool MS5837_30BA::IsEnabled()
{
    return m_enabled;
}

bool MS5837_30BA::GetLock()
{
    return m_pI2C->LockAddress( m_address );
}

void MS5837_30BA::FreeLock()
{
    m_pI2C->FreeAddress( m_address );
}

EResult MS5837_30BA::SetOversampleRate( EOversampleRate rateIn )
{
    // Change the oversampling rate
    m_osr = rateIn;

    // Reset sensor, since we might be in the middle of a conversion.
    Transition( EState::UNINITIALIZED );

    return EResult::RESULT_SUCCESS;
}

EResult MS5837_30BA::SetWaterType( EWaterType typeIn )
{
    // Update the water modifier. No need to adjust state machine.
    m_waterMod = ( ( typeIn == EWaterType::FRESH ) ? kWaterModFresh : kWaterModSalt );

    return EResult::RESULT_SUCCESS;
}

uint32_t MS5837_30BA::GetUpdatePeriod()
{
    // Two conversion periods
    return 2 * kOSRInfo[ static_cast<uint8_t>( m_osr ) ].conversionTime_ms;
}

// --------------------------------------------------------------
// Private Methods
// --------------------------------------------------------------

void MS5837_30BA::Transition( EState stateIn )
{
    // Set new state
    m_state = stateIn;
}

void MS5837_30BA::DelayedTransition( EState nextState, uint32_t millisIn )
{
    // Set delay info
    m_state                 = EState::DELAY;
    m_delay.nextState       = nextState;
    m_delay.delayTime_ms    = millisIn;
    m_delay.timer.Reset(); 
}

EResult MS5837_30BA::Cmd_Reset()
{
    // Attempt to reset
    if( WriteByte( CMD_RESET ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        return EResult::RESULT_SUCCESS;
    }
}

EResult MS5837_30BA::Cmd_ReadCalibrationData()
{
    uint8_t coeffs[ 2 ];

	// Read sensor coefficients
	for( uint8_t i = 0; i < 7; ++i )
	{
        i2c::EI2CResult ret = ReadRegisterBytes( CMD_PROM_READ_BASE + ( i * 2 ), coeffs, 2 );

		if( ret )
		{
            // I2C Failure
            return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
		}

        // Combine high and low bytes
        m_coeffs[i] = ( ( ( uint16_t )coeffs[ 0 ] << 8 ) | coeffs[ 1 ] );
	}

    uint8_t readCRC = ( m_coeffs[ 0 ] >> 12 );

    // Compare read CRC4 to calculated CRC4
    if( readCRC == CalculateCRC4() )
    {
		return EResult::RESULT_SUCCESS;
	}
	else
	{
		return (EResult)m_results.AddResult( EResult::RESULT_ERR_CRC_MISMATCH );
	}
}

EResult MS5837_30BA::Cmd_StartPresConversion()
{
    if( WriteByte( CMD_PRES_CONV_BASE + kOSRInfo[ static_cast<uint8_t>( m_osr ) ].commandMod ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        // Success
        return EResult::RESULT_SUCCESS;
    }
}

EResult MS5837_30BA::Cmd_StartTempConversion()
{
    if( WriteByte( CMD_TEMP_CONV_BASE + kOSRInfo[ static_cast<uint8_t>( m_osr ) ].commandMod ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        // Success
        return EResult::RESULT_SUCCESS;
    }
}

EResult MS5837_30BA::Cmd_ReadPressure()
{
    uint8_t bytes[ 3 ];

    i2c::EI2CResult ret = ReadRegisterBytes( CMD_ADC_READ, bytes, 3 );

    if( ret )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }

    // Combine the bytes into one integer for the final result
    m_D1 = ( ( uint32_t )bytes[ 0 ] << 16 ) + ( ( uint32_t )bytes[ 1 ] << 8 ) + ( uint32_t )bytes[ 2 ];

    return EResult::RESULT_SUCCESS;
}

EResult MS5837_30BA::Cmd_ReadTemperature()
{
    uint8_t bytes[ 3 ];

    i2c::EI2CResult ret = ReadRegisterBytes( CMD_ADC_READ, bytes, 3 );

    if( ret )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }

	// Combine the bytes into one integer for the final result
	m_D2 = ( ( uint32_t )bytes[ 0 ] << 16 ) + ( ( uint32_t )bytes[ 1 ] << 8 ) + ( uint32_t )bytes[ 2 ];

    return EResult::RESULT_SUCCESS;
}

uint8_t MS5837_30BA::CalculateCRC4()
{
    int cnt;            // simple counter
    uint32_t n_rem = 0; // crc remainder
    uint8_t n_bit;
    
    // Replace the CRC byte with 0
    m_coeffs[0] = ((m_coeffs[0]) & 0x0FFF); 

    // Leftover value from the MS5803 series, set to 0
    m_coeffs[7] = 0;
    
    // Loop through each byte in the coefficients
    for( cnt = 0; cnt < 16; ++cnt )
    { 
        // Choose LSB or MSB
        if( ( cnt % 2 ) == 1 ) 
        {
            n_rem ^= (uint16_t)( ( m_coeffs[ cnt >> 1 ] ) & 0x00FF );
        }
        else
        {
            n_rem ^= (uint16_t)( m_coeffs[ cnt >> 1 ] >> 8 );
        }
            
        for( n_bit = 8; n_bit > 0; --n_bit )
        {
            if( n_rem & 0x8000 )
            {
                n_rem = ( n_rem << 1 ) ^ 0x3000;
            }
            else 
            {
                n_rem = ( n_rem << 1 );
            }
        }
    }
    
    // Final 4-bit remainder is the CRC value
    n_rem = ( ( n_rem >> 12 ) & 0x000F ); 
    
    return ( n_rem ^ 0x00 );
}

void MS5837_30BA::ProcessData()
{
    // Calculate base terms
	m_dT      = (int32_t)m_D2 - ( (int32_t)m_coeffs[5] * POW_2_8 );
	m_TEMP    = 2000 + ( ( (int64_t)m_dT * (int32_t)m_coeffs[6] ) / POW_2_23 );
	
	m_OFF     = ( (int64_t)m_coeffs[2] * POW_2_16 ) + ( ( (int64_t)m_coeffs[4] * m_dT ) / POW_2_7 );
	m_SENS    = ( (int64_t)m_coeffs[1] * POW_2_15 ) + ( ( (int64_t)m_coeffs[3] * m_dT ) / POW_2_8 );
	
	// Calculate intermediate values depending on temperature
	if( m_TEMP < 2000 )
	{
	    // Temps < 20C
		m_Ti      = 3 * ( ( int64_t )m_dT * m_dT ) / POW_2_33;
		m_OFFi    = 3 * ( ( m_TEMP - 2000 ) * ( m_TEMP - 2000 ) ) / 2 ;
		m_SENSi   = 5 * ( ( m_TEMP - 2000 ) * ( m_TEMP - 2000 ) ) / 8 ;
		
		// Additional compensation for very low temperatures (< -15C)
    	if( m_TEMP < -1500 )
    	{
    		// For 14 bar model
    		m_OFFi    = m_OFFi + 7 * ( ( m_TEMP + 1500 ) * ( m_TEMP + 1500 ) );
    		m_SENSi   = m_SENSi + 4 * ( ( m_TEMP + 1500 ) * ( m_TEMP + 1500 ) );
    	}
	}
	else
	{
	    m_Ti      = 2 * ( ( int64_t )m_dT * m_dT ) / POW_2_37;
		m_OFFi    = 1 * ( ( m_TEMP - 2000 ) * ( m_TEMP - 2000 ) ) / 16;
		m_SENSi   = 0;
	}
	
	m_OFF2    = m_OFF - m_OFFi;
	m_SENS2   = m_SENS - m_SENSi;
	
	m_TEMP2 = (m_TEMP - m_Ti);
	m_P = ( ( ( ( (int64_t)m_D1 * m_SENS2 ) / POW_2_21 ) - m_OFF2 ) / POW_2_13 );
	
    // Create data sample with calculated parameters
    m_data.Update(  ( (float)m_TEMP2 / 100.0f ),    // Temperature
                    ( (float)m_P / 10.0f ),         // Pressure
                    m_waterMod );
}

// I2C call wrappers
i2c::EI2CResult MS5837_30BA::WriteByte( uint8_t registerIn )
{
    return m_pI2C->WriteByte( m_address, registerIn );
}

i2c::EI2CResult MS5837_30BA::ReadRegisterBytes( uint8_t registerIn, uint8_t *dataOut, uint8_t numBytesIn )
{
    return m_pI2C->ReadRegisterBytes( m_address, registerIn, dataOut, numBytesIn );
}