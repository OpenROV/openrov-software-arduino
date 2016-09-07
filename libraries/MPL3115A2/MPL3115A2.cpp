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

int32_t MPL3115A2::Initialize() 
{
    m_isInitialized = false;

    delay( 500 );

    //Verify that the sensor is up and running
    m_lastRetcode = VerifyChipId();
}

/***************************************************************************
    PRIVATE FUNCTIONS
 ***************************************************************************/



int32_t MPL3115A2::VerifyChipId()
{
    //Read the chip id
    uint8_t id;

    m_lastRetcode = ReadByte( MPL3115A2_REGISTER::WHO_AM_I, id );

    if( m_lastRetcode != I2C::ERetCode::SUCCESS )
    {
        return m_lastRetcode;
    }

    //Check to see if it matches the proper ID (0xC4)
    if( id != 0xC4 )
    {
        return -1;
    }

    return m_lastRetcode;
}


int32_t MPL3115A2::WriteByte( MPL3115A2_ADDRESS addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t MPL3115A2::ReadByte( MPL3115A2_ADDRESS addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}