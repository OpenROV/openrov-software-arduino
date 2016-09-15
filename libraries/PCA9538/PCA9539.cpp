/* 
 PCA9539 GPIO Expander Library
 By: OpenROV
 Date: September 14, 2016
*/

#include <Arduino.h>
#include <CI2C.h>

#include "PCA9539.h"

using namespace pca9539;

PCA9539::PCA9539( CI2C* i2cInterfaceIn )
    : m_i2cAddress( pca9539::PCA9539_ADDRESS )
    , m_pI2C( i2cInterfaceIn )
    , m_gpioDirection( 0xFF ) //All inputs
    , m_gpioState( 0x00 ) //All low
{
    
}

ERetCode PCA9539::Initialize()
{
    Serial.println( "PCA9539.status: INIT" );

    m_gpioDirection = 0x00; //All outputs

    uint8_t value;
    //Read it
    auto ret = ReadByte( PCA9539_REGISTER::CONFIG, value );
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }
    Serial.println(value, HEX);

    //Write it
    ret = WriteByte( PCA9539_REGISTER::CONFIG, m_gpioDirection);
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }

    //Read it
    ret = ReadByte( PCA9539_REGISTER::CONFIG, value );
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }
    Serial.println(value, HEX);


    m_gpioState |= (1 << 5);

    //Write it
    ret = WriteByte( PCA9539_REGISTER::OUTPUT, m_gpioState);
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED;
    }



    return ERetCode::SUCCESS;
}
value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);

if(HIGH)
{
    value = _gpioState |= (1 << pin);
}
else
{
    _gpioState &= ~(1 << pin)
}




/***************************************************************************
    PRIVATE FUNCTIONS
 ***************************************************************************/
int32_t PCA9539::WriteByte( PCA9539_REGISTER addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t PCA9539::ReadByte( PCA9539_REGISTER addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}
int32_t PCA9539::ReadNBytes( PCA9539_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn )
{
    return (int32_t)m_pI2C->ReadBytes( m_i2cAddress, (uint8_t)addressIn, dataOut, byteCountIn );
}

