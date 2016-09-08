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


MPL3115A2::MPL3115A2( CI2C *i2cInterfaceIn, int32_t sensorIdIn = -1, uint8_t addressIn = MPL3115A2_ADDRESS )
    : m_pI2C( i2cInterfaceIn )
    , m_sensorId( sensorIdIn )
    , m_i2cAddress( addressIn )
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

ERetCode SetMode( EMode modeIn )
{
    switch (modeIn)
    {
        case EMode::BAROMETER:
        {
            return SetModeBarometer();
        }
        case EMode::ALTIMETER:
        {
            return SetModeAltimter();
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

/***************************************************************************
    PRIVATE FUNCTIONS
 ***************************************************************************/

ERetCode MPL3115A2::VerifyChipId()
{
    //Read the chip id
    uint8_t id;

    auto ret = ReadByte( MPL3115A2_REGISTER::WHO_AM_I, id );

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
    returnCode = ReadByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear the altimeter bit
    tempSetting &= ~(1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;
}

ERetCode MPL3115A2::SetModeAltimter()
{
    int32_t returnCode;
    
    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set the altimeter bit
    tempSetting |= (1<<7);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
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
    returnCode = ReadByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Clear SBYB bit for Standby mode
    tempSetting &= ~(1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
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
    returnCode = ReadByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set SBYB bit for Active mode
    tempSetting |= (1<<0);

    //And write it to the register
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
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
    int32_t returnCode;

    //Read the current settings
    uint8_t tempSetting;
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }
    
    //Clear the one shot bit
    tempSetting &= ~(1<<1);
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Reat the current settings, just to be safe :)
    returnCode = ReadByte( MPL3115A2_REGISTER::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Set the overshot bit
    tempSetting |= (1<<1);
    returnCode = WriteByte( MPL3115A2::CONTROL_REGISTER_1, tempSetting );
    if( returnCode != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    return ERetCode::SUCCESS;    
}

int32_t MPL3115A2::WriteByte( MPL3115A2_ADDRESS addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t MPL3115A2::ReadByte( MPL3115A2_ADDRESS addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}