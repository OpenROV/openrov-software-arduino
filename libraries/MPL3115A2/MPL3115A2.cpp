/* 
 MPL3115A2 Barometric Pressure Sensor Library

 Get pressure, altitude and temperature from the MPL3115A2 sensor.
 
 September 6, 2016: Modified for use in OpenROV's Software 

*/

#include "MPL3115A2.h"

using namespace mpl3115a2;


MPL3115A2::MPL3115A2( I2C *i2cInterfaceIn )
    : m_i2cAddress( mpl3115a2::MPL3115A2_ADDRESS )
    , m_pI2C( i2cInterfaceIn )
{

}

ERetCode MPL3115A2::Initialize() 
{
    m_isInitialized = false;

    delay( 500 );

    //Verify that the sensor is up and running
    if( VerifyChipId() != ERetCode::SUCCESS )
    {
        //We were unable to verify this sensor
        return ERetCode::FAILED;
    }

    m_isInitialized = true;
    return ERetCode::SUCCESS;    
}

ERetCode MPL3115A2::SetMode( EMode modeIn )
{
    switch (modeIn)
    {
        case EMode::BAROMETER:
        {
            return SetModeBarometer();
        }
        case EMode::ALTIMETER:
        {
            return SetModeAltimeter();
        }
        case EMode::STANDBY:
        {
            return SetModeStandby();
        }
        case EMode::ACTIVE:
        {
            return SetModeActive();
        }
        default:
        {
            return ERetCode::FAILED;
        }
    }
}

//Call with a rate from 0 to 7. See page 33 for table of ratios.
//Sets the over sample rate. Datasheet calls for 128 but you can set it 
//from 1 to 128 samples. The higher the oversample rate the greater
//the time between data samples.
ERetCode MPL3115A2::SetOversampleRatio( EOversampleRatio osrIn )
{
    int32_t returnCode;
    auto sampleRate = static_cast<int>( osrIn );

    //Align it for the control register
    sampleRate <<= 3;

    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear out old Oversample bits
    tempSetting &= B11000111;

    //Mask new sample rate
    tempSetting |= sampleRate;

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    return ERetCode::SUCCESS;
}

//Enables the pressure and temp measurement event flags so that we can
//test against them. This is recommended in datasheet during setup.
ERetCode MPL3115A2::EnableEventFlags()
{
    // Enable all three pressure and temp event flags 
    auto ret = WriteByte( MPL3115A2_REGISTER::PT_DATA_CFG, 0x07);
    if( ret != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    return ERetCode::SUCCESS;
}

//Reads the current pressure in kPa
//Unit must be set in barometric pressure mode
ERetCode MPL3115A2::ReadPressure( float& pressureOut )
{
    int32_t retCode;

    //Check the PDR (pressure data ready) bit to see if we need to toggle oneshot
    uint8_t status;

    retCode = ReadByte( MPL3115A2_REGISTER::STATUS, status );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    if( ( status & ( 1<<2 ) ) == 0 )
    {
        auto ret = ToggleOneShot();
        if( retCode != ERetCode::SUCCESS )
        {
            return ERetCode::FAILED_ONESHOT;
        }
    }

    //Wait for PDR bit, indicates we have new pressure data
    uint16_t counter = 0;
    uint8_t pdr;
    while( ( pdr & (1<<2) ) == 0 )
    {
        retCode = ReadByte( MPL3115A2_REGISTER::STATUS, pdr );
        if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
        {
            return ERetCode::FAILED;
        }

        //Timeout
        if( ++counter > 600 )
        {
            return ERetCode::TIMED_OUT;
        }
    }

    //Read the pressure registers
    uint8_t buffer[3] = {};

    retCode = ReadNBytes( MPL3115A2_REGISTER::PRESSURE_OUT_MSB, buffer, 3 );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    auto msb = buffer[0];
    auto csb = buffer[1];
    auto lsb = buffer[2];

    //Take another reading
    auto ret = ToggleOneShot();
    if( retCode != ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Pressure comes back as a left shifted 20 bit-number
    uint32_t totalPressure = (((uint32_t)(msb)<<16) | (((uint32_t)csb)<<8) | (uint32_t)lsb );

    //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
    totalPressure >>= 6;

    //Bits 5/4 represent the fractional component
    lsb &= 0x30; //B00110000
    
    //Get it right aligned
    lsb >>= 4;

    //Turn it into a fraction
    float decimalPressure = (float)lsb/4.0f;

    pressureOut = (float)totalPressure + decimalPressure;

    return ERetCode::SUCCESS;
}

//Reads the current altitude in meters
//Unit must be set in altitude mode
ERetCode MPL3115A2::ReadAltitude( float& altitudeOut )
{
    int32_t retCode;

    //Toggle the Overshot bit which causes the sensor to take another reading
    auto ret = ToggleOneShot();
    if( retCode != ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Wait for PDR bit, indicates we have new altitude data
    uint16_t counter = 0;
    uint8_t pdr;
    while( ( pdr & (1<<1) ) == 0 )
    {
        retCode = ReadByte( MPL3115A2_REGISTER::STATUS, pdr );
        if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
        {
            return ERetCode::FAILED;
        }

        //Timeout
        if( ++counter > 600 )
        {
            return ERetCode::TIMED_OUT;
        }
    }

    //Read the altitde registers
    uint8_t buffer[3] = {};

    retCode = ReadNBytes( MPL3115A2_REGISTER::PRESSURE_OUT_MSB, buffer, 3 );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    auto msb = buffer[0];
    auto csb = buffer[1];
    auto lsb = buffer[2];

    // The least significant bytes l_altitude and l_temp are 4-bit,
	// fractional values, so you must cast the calulation in (float),
	// shift the value over 4 spots to the right and divide by 16 (since 
	// there are 16 values in 4-bits). 
	float tempCSB = (lsb>>4)/16.0;

	altitudeOut = (float)( (msb << 8) | csb) + tempCSB;

    return ERetCode::SUCCESS;
}

ERetCode MPL3115A2::ReadPressureAndTemp( float& pressureOut, float& tempOut )
{
    int32_t retCode;

    //Check the PDR (pressure data ready) bit to see if we need to toggle oneshot
    uint8_t status;

    retCode = ReadByte( MPL3115A2_REGISTER::STATUS, status );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    if( ( status & ( 1<<2 ) ) == 0 )
    {
        auto ret = ToggleOneShot();
        if( retCode != ERetCode::SUCCESS )
        {
            return ERetCode::FAILED_ONESHOT;
        }
    }

    //Wait for PDR bit, indicates we have new pressure data
    uint16_t counter = 0;
    uint8_t pdr;
    while( ( pdr & (1<<2) ) == 0 )
    {
        retCode = ReadByte( MPL3115A2_REGISTER::STATUS, pdr );
        if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
        {
            return ERetCode::FAILED;
        }

        //Timeout
        if( ++counter > 600 )
        {
            return ERetCode::TIMED_OUT;
        }
    }

    //Read the pressure registers
    uint8_t pressureBuffer[3] = {};

    retCode = ReadNBytes( MPL3115A2_REGISTER::PRESSURE_OUT_MSB, pressureBuffer, 3 );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Read the Temp registers
    uint8_t tempBuffer[2] = {};
    retCode = ReadNBytes( MPL3115A2_REGISTER::TEMP_OUT_MSB, tempBuffer, 2 );
    if( retCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Take another reading
    auto ret = ToggleOneShot();
    if( retCode != ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Pressure conversion
    auto pressureMSB = pressureBuffer[0];
    auto pressureCSB = pressureBuffer[1];
    auto pressureLSB = pressureBuffer[2];

    //Pressure comes back as a left shifted 20 bit-number
    uint32_t totalPressure = (((uint32_t)(pressureMSB)<<16) | (((uint32_t)pressureCSB)<<8) | (uint32_t)pressureLSB );

    //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
    totalPressure >>= 6;

    //Bits 5/4 represent the fractional component
    pressureLSB &= 0x30; //B00110000
    
    //Get it right aligned
    pressureLSB >>= 4;

    //Turn it into a fraction
    float decimalPressure = (float)pressureLSB/4.0f;

    pressureOut = (float)totalPressure + decimalPressure;


    //Temp conversion
    auto tempMSB = tempBuffer[0];
    auto tempLSB = tempBuffer[1];

    // Get temperature, the 12-bit temperature measurement in °C is comprised of a signed integer component and a fractional component.
    // The signed 8-bit integer component is located in OUT_T_MSB. 
    // The fractional component is located in bits 7-4 of OUT_T_LSB.
    // Bits 3-0 of OUT_T_LSB are not used.

    tempOut = (float)((signed char) tempMSB) + (float)(tempLSB >> 4) * 0.0625;

    return ERetCode::SUCCESS;
}


/***************************************************************************
    PRIVATE FUNCTIONS
 ***************************************************************************/

ERetCode MPL3115A2::VerifyChipId()
{
    //Read the chip id
    uint8_t id;

    auto ret = ReadByte( MPL3115A2_REGISTER::WHO_AM_I, id );

    if( ret != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Check to see if it matches the proper ID (0xC4)
    if( id != 0xC4 )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

ERetCode MPL3115A2::SetModeBarometer()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t setting;

    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, setting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear the altimeter bit
    setting &= ~(1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, setting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

ERetCode MPL3115A2::SetModeAltimeter()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set the altimeter bit
    tempSetting |= (1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

//Puts the sensor in standby mode
//This is needed so that we can modify the major control registers
ERetCode MPL3115A2::SetModeStandby()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear SBYB bit for Standby mode
    tempSetting &= ~(1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS; 
}

//Puts the sensor in active mode
//This is needed so that we can modify the major control registers
ERetCode MPL3115A2::SetModeActive()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set SBYB bit for Active mode
    tempSetting |= (1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

//Clears and then sets the OST bit which causes the sensor to take another readings
//Needed to sample faster than 1Hz
ERetCode MPL3115A2::ToggleOneShot()
{

    int32_t returnCode;

    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }
    
    //Clear the one shot bit
    tempSetting &= ~(1<<1);
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Reat the current settings, just to be safe :)
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Set the overshot bit
    tempSetting |= (1<<1);
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != i2c::EI2CResult::RESULT_SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    return ERetCode::SUCCESS;    
}

int32_t MPL3115A2::WriteByte( MPL3115A2_REGISTER addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteRegisterByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t MPL3115A2::ReadByte( MPL3115A2_REGISTER addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadRegisterByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}
int32_t MPL3115A2::ReadNBytes( MPL3115A2_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn )
{
    return (int32_t)m_pI2C->ReadRegisterBytes( m_i2cAddress, (uint8_t)addressIn, dataOut, byteCountIn );
}