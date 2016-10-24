// Includes
#include "BNO055.h"

using namespace bno055;

BNO055::BNO055( I2C *i2cInterfaceIn, EAddress addressIn )
    : m_pI2C( i2cInterfaceIn )
    , m_address( ( addressIn == EAddress::ADDRESS_A ? I2C_ADDRESS_A : I2C_ADDRESS_B ) )
{
}

// --------------------------------------------------------------
// Public API
// --------------------------------------------------------------

void BNO055::Tick()
{
    switch( m_state )
    {
        // -----------------------------
        // Active operating conditions
        // -----------------------------
        case EState::ACTIVE:
        {
            if( m_calibTimer.HasElapsed( kCalUpdateRate_ms ) )
            {
                if( Cmd_ReadCalibrationState() )
                {
                    m_results.AddResult( EResult::RESULT_ERR_READING_CALIB );
                }
            }

            if( m_dataTimer.HasElapsed( kModeInfo[ (uint8_t)m_mode ].updatePeriod ) )
            {
                if( Cmd_ReadEulerData() )
                {
                    m_results.AddResult( EResult::RESULT_ERR_READING_EULER );
                }
            }

            break;
        }

        // -----------------------------
        // Initialization routines
        // -----------------------------
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

        case EState::POWER_ON_RESET:
        {
            // Wait for POR to complete
            DelayedTransition( EState::VERIFYING_CHIP_INFO, kPowerOnResetDelay_ms );
            break;
        }

        case EState::USER_RESET:
        {
            // Attempt to reset
            if( Cmd_Reset() )
            {
                // Failure, retry
                HardReset();
            }
            else
            {
                // Sucess
                Transition( EState::POWER_ON_RESET );
            }

            break;
        }

        case EState::VERIFYING_CHIP_INFO:
        {
            // Attempt to reset
            if( Cmd_VerifyChipID() )
            {
                HardReset();
                break;
            }

            // Check POST results
            if( Cmd_VerifyPostResults() )
            {
                HardReset();
                break;
            }

            // Get software revision info
            if( Cmd_ReadRevisionInfo() )
            {
                HardReset();
                break;
            }

            // Sucess
            Transition( EState::VERIFYING_CONFIG_MODE );
        }

        case EState::VERIFYING_CONFIG_MODE:
        {
            // Attempt to reset
            if( Cmd_ReadOperatingMode() )
            {
                // Failure, retry
                HardReset();
            }
            else
            {
                if( m_opMode == EOpMode::OPERATION_MODE_CONFIG )
                {
                    // Sucess
                    Transition( EState::CONFIGURING );
                }
                else
                {
                    // Failure, retry
                    m_results.AddResult( EResult::RESULT_ERR_INCORRECT_STATE_AFTER_POR );
                    HardReset();
                }   
            }

            break;
        }

        case EState::CONFIGURING:
        {
            // Attempt to reset
            if( Cmd_ConfigureSensor() )
            {
                // Failure, retry
                HardReset();
            }
            else
            {
                // Sucess
                DelayedTransition( EState::VERIFYING_ACTIVE, kModeSwitch_ms );
            }

            break;
        }

        case EState::VERIFYING_ACTIVE:
        {
            // Attempt to reset
            if( Cmd_ReadOperatingMode() )
            {
                // Failure, retry
                HardReset();
            }
            else
            {
                if( m_opMode == kModeInfo[ (uint8_t)m_mode ].opMode )
                {
                    // Sucess
                    Transition( EState::ACTIVE );
                }
                else
                {
                    // Failure, retry
                    HardReset();
                }   
            }

            break;
        }

        // -----------------------------
        // Disabled
        // -----------------------------
        case EState::DISABLED:
        default:
        {
            // Do nothing
            break;
        }
    }
}

void BNO055::HardReset()
{
    // Perform delayed transition to uninitialized
    DelayedTransition( EState::USER_RESET, kRetryDelay_ms );

    // Increment hard reset counter
    m_results.AddResult( EResult::RESULT_ERR_HARD_RESET );

    // Reset timers
    m_calibTimer.Reset();
    m_dataTimer.Reset();

    // Clear the read error counter
    ClearResultCount( EResult::RESULT_ERR_READING_EULER );
}

void BNO055::FullReset()
{
    // Perform delayed transition to uninitialized
    DelayedTransition( EState::USER_RESET, kRetryDelay_ms );

    // Reset timers
    m_calibTimer.Reset();
    m_dataTimer.Reset();
    
    // Clear all results
    m_results.Clear();

    // Re-enable device
    m_enabled = true;
}

uint32_t BNO055::GetResultCount( EResult resultTypeIn )
{
    return m_results.GetResultCount( resultTypeIn );
}

void BNO055::ClearResultCount( EResult resultTypeIn )
{
    m_results.ClearResult( resultTypeIn );
}

void BNO055::Disable()
{
    // Disable device entirely
    Transition( EState::DISABLED );
    m_enabled = false;
}

bool BNO055::IsEnabled()
{
    return m_enabled;
}

EResult BNO055::SetMode( EMode modeIn )
{
    if( modeIn < EMode::_MODE_COUNT )
    {
        // Change the operating mode
        m_mode = modeIn;

        // Reset sensor state machine
        Transition( EState::USER_RESET );

        return EResult::RESULT_SUCCESS;
    }
    else
    {
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_INVALID_MODE );
    }
}

EMode BNO055::GetMode()
{
    return m_mode;
}

uint32_t BNO055::GetUpdatePeriod()
{
    // Two conversion periods
    return kModeInfo[ (uint8_t)m_mode ].updatePeriod;
}

// --------------------------------------------------------------
// Private Methods
// --------------------------------------------------------------

void BNO055::Transition( EState stateIn )
{
    // Set new state
    m_state = stateIn;
}

void BNO055::DelayedTransition( EState nextState, uint32_t millisIn )
{
    // Set delay info
    m_state                 = EState::DELAY;
    m_delay.nextState       = nextState;
    m_delay.delayTime_ms    = millisIn;
    m_delay.timer.Reset();
}

EResult BNO055::Cmd_Reset()
{
    // Attempt to reset
    if( WriteRegisterByte( ERegister::REG_SYS_TRIGGER_ADDR, 0x20 ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        return EResult::RESULT_SUCCESS;
    }
}

EResult BNO055::Cmd_VerifyChipID()
{
    // Read ID
    if( ReadRegisterByte( ERegister::REG_CHIP_ID_ADDR, &m_chipID ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        if( m_chipID == BNO_ID )
        {
            return EResult::RESULT_SUCCESS;
        }
        else
        {
            return (EResult)m_results.AddResult( EResult::RESULT_ERR_INVALID_ID );
        }
    }
}

EResult BNO055::Cmd_VerifyPostResults()
{
    // Read ID
    if( ReadRegisterByte( ERegister::REG_SELFTEST_RESULT_ADDR, &m_postResults ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        // Bit 0 = Accelerometer self test
        // Bit 1 = Magnetometer self test
        // Bit 2 = Gyroscope self test
        // Bit 3 = MCU self test
        if( ( m_postResults & 0x0F ) == 0x0F )
        {
            return EResult::RESULT_SUCCESS;
        }
        else
        {
            return (EResult)m_results.AddResult( EResult::RESULT_ERR_INVALID_ID );
        }
    }
}

EResult BNO055::Cmd_ReadRevisionInfo()
{
    uint8_t a;
    uint8_t b;

	if( ReadRegisterByte( REG_ACCEL_REV_ID_ADDR, &m_revInfo.accel ) )     { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
	if( ReadRegisterByte( REG_MAG_REV_ID_ADDR, &m_revInfo.mag ) )         { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
	if( ReadRegisterByte( REG_GYRO_REV_ID_ADDR, &m_revInfo.gyro ) )       { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
	if( ReadRegisterByte( REG_BL_REV_ID_ADDR, &m_revInfo.bootloader ) )   { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
	if( ReadRegisterByte( REG_SW_REV_ID_LSB_ADDR, &a ) )                  { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
    if( ReadRegisterByte( REG_SW_REV_ID_MSB_ADDR, &b ) )                  { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }

	// Combine the LSB and MSB
	m_revInfo.software = ( ( ( uint16_t )b ) << 8 ) | ( ( uint16_t )a );

    return EResult::RESULT_SUCCESS;
}

EResult BNO055::Cmd_ReadOperatingMode()
{
    uint8_t opMode = 0;

    // Read Op Mode Register
    if( ReadRegisterByte( ERegister::REG_OPR_MODE_ADDR, &opMode ) )
    {
        // I2C Failure
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }
    else
    {
        // OPR_MODE is the last 4 bits of the register
        m_opMode = (EOpMode)(opMode & 0x0F);
        return EResult::RESULT_SUCCESS;
    }
}

EResult BNO055::Cmd_ConfigureSensor()
{
    // Set up units and orientation
	uint8_t unitSelect =	( 0 << 7 ) |	// Orientation = Android
	                        ( 0 << 4 ) |	// Temperature = Celsius
	                        ( 0 << 2 ) |	// Euler = Degrees
	                        ( 0 << 1 ) |	// Gyro = Deg/s
	                        ( 1 << 0 );		// Accelerometer = mg

	if( WriteRegisterByte( REG_UNIT_SEL_ADDR, unitSelect ) )    { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }

	// Wires forward, sensors down
    // 0x24 = Default axis assignment setup
    // 0x00 = Default axis direction setup
    // 0x04 = X axis direction flipped
	if( WriteRegisterByte( REG_AXIS_MAP_CONFIG_ADDR, 0x24 ) )   { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }
	if( WriteRegisterByte( REG_AXIS_MAP_SIGN_ADDR, 0x03 ) )     { return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION ); }

    // Move to selected fusion mode
    if( WriteRegisterByte( REG_OPR_MODE_ADDR, kModeInfo[ (uint8_t)m_mode ].opMode ) )
    { 
        return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
    }

    return EResult::RESULT_SUCCESS;
}

EResult BNO055::Cmd_ReadCalibrationState()
{
    uint8_t calData;

	if( ReadRegisterByte( REG_CALIB_STAT_ADDR, &calData ) )
	{
		return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
	}

    m_calibration.Update(   (ECalibration)((calData>>2)&0x03),      // Accel
                            (ECalibration)((calData>>4)&0x03),      // Gyro
                            (ECalibration)(calData&0x03),           // Mag
                            (ECalibration)((calData>>6)&0x03) );    // System

	return EResult::RESULT_SUCCESS;
}

EResult BNO055::Cmd_ReadEulerData()
{
    uint8_t buffer[6];

    int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;

    // Read vector data (6 bytes)
	
	if( ReadRegisterBytes( EVectorAddress::VECTOR_EULER, buffer, 6 ) )
	{
		return (EResult)m_results.AddResult( EResult::RESULT_ERR_I2C_TRANSACTION );
	}

	x = ( ( int16_t )buffer[0] ) | ( ( ( int16_t )buffer[1] ) << 8 );
	y = ( ( int16_t )buffer[2] ) | ( ( ( int16_t )buffer[3] ) << 8 );
	z = ( ( int16_t )buffer[4] ) | ( ( ( int16_t )buffer[5] ) << 8 );

    // 1 Degree = 16.0f
    // (rpy)
    m_data.Update( ( ( (float)y ) / 16.0f ), -( ( (float)z ) / 16.0f ), ( ( (float)x ) / 16.0f ) );

    return EResult::RESULT_SUCCESS;
}

// I2C call wrappers
i2c::EI2CResult BNO055::WriteRegisterByte( uint8_t registerIn, uint8_t dataIn )
{
    return m_pI2C->WriteRegisterByte( m_address, registerIn, dataIn );
}

i2c::EI2CResult BNO055::ReadRegisterByte( uint8_t registerIn, uint8_t *dataOut )
{
    return m_pI2C->ReadRegisterByte( m_address, registerIn, dataOut );
}

i2c::EI2CResult BNO055::ReadRegisterBytes( uint8_t registerIn, uint8_t *dataOut, uint8_t numBytesIn )
{
    return m_pI2C->ReadRegisterBytes( m_address, registerIn, dataOut, numBytesIn );
}