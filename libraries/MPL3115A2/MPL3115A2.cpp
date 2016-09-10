/* 
 MPL3115A2 Barometric Pressure Sensor Library
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 24th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Get pressure, altitude and temperature from the MPL3115A2 sensor.
 
 September 6, 2016: Modified for use in OpenROV's Software 

*/

#include <Arduino.h>
#include <CI2C.h>

#include "MPL3115A2.h"

using namespace mpl3115a2;


MPL3115A2::MPL3115A2( CI2C *i2cInterfaceIn )
    : m_i2cAddress( mpl3115a2::MPL3115A2_ADDRESS )
    , m_pI2C( i2cInterfaceIn )
    , m_sensorId( 420 )
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
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear out old Oversample bits
    tempSetting &= B11000111;

    //Mask new sample rate
    tempSetting |= sampleRate;

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
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
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    return ERetCode::SUCCESS;
}

//Reads the current pressure in Pa
//Unit must be set in barometric pressure mode
ERetCode MPL3115A2::ReadPressure( float& pressureOut )
{
    Serial.println( "In read pressure loop" );
    ToggleOneShot();

    int32_t returnCode;
    uint8_t pdr;

    //Wait for PDR bit, indicates we have new pressure data
    auto counter = 0;
    
    returnCode = ReadByte( MPL3115A2_REGISTER::STATUS, pdr );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_PRESSURE_READ;
    }
    
    // while( ( pdr & (1<<2) ) == 0 )
    // {
    //     Serial.println( counter );


    //     if( ++counter > 100 )
    //     {
    //         return ERetCode::TIMED_OUT;
    //     }
    //     delay(1);
    // }

    //Read pressure registers
    uint8_t buffer[3];
    memset( buffer, 0, 3);

    returnCode = ReadNBytes( MPL3115A2_REGISTER::PRESSURE_OUT_MSB, buffer, 3 );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_PRESSURE_READ;
    }

    //Toggle the OST bit causing the sensor to immediately take another reading
    ToggleOneShot();

    auto msb = buffer[0];
    auto csb = buffer[1];
    auto lsb = buffer[2];
    
    // Pressure comes back as a left shifted 20 bit number
    uint64_t pressure = (uint64_t)msb<<16 | (uint64_t)csb<<8 | (uint64_t)lsb;

    //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
    pressure >>= 6;

    //Bits 5/4 represent the fractional component
    lsb &= B00110000;

    //Get it right aligned
    lsb >>= 4;

    //Turn it into fraction
    float pressure_decimal = (float)lsb/4.0; 

	pressureOut = (float)pressure + pressure_decimal;

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
    Serial.println(id, HEX);

    if( ret != I2C::ERetCode::SUCCESS )
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
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear the altimeter bit
    tempSetting &= ~(1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    m_mode = tempSetting;

    return ERetCode::SUCCESS;
}

ERetCode MPL3115A2::SetModeAltimeter()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set the altimeter bit
    tempSetting |= (1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
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
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear SBYB bit for Standby mode
    tempSetting &= ~(1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
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
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set SBYB bit for Active mode
    tempSetting |= (1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

//Clears and then sets the OST bit which causes the sensor to take another readings
//Needed to sample faster than 1Hz
ERetCode MPL3115A2::ToggleOneShot()
{
    Serial.println( "Toggling one shot." );
    Serial.print( "Mode: " );
    Serial.println( m_mode );
    int32_t returnCode;

    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }
    
    //Clear the one shot bit
    tempSetting &= ~(1<<1);
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Reat the current settings, just to be safe :)
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    //Set the overshot bit
    tempSetting |= (1<<1);
    returnCode = WriteByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_ONESHOT;
    }

    return ERetCode::SUCCESS;    
}

int32_t MPL3115A2::WriteByte( MPL3115A2_REGISTER addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t MPL3115A2::ReadByte( MPL3115A2_REGISTER addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}
int32_t MPL3115A2::ReadNBytes( MPL3115A2_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn )
{
    return (int32_t)m_pI2C->ReadBytes( m_i2cAddress, (uint8_t)addressIn, dataOut, byteCountIn );
}