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
{
    
}

ERetCode PCA9539::Initialize()
{
    Serial.println( "PCA9539.status: INIT" );
}





/***************************************************************************
    PRIVATE FUNCTIONS
 ***************************************************************************/
int32_t PCA9539::WriteByte( uint8_t addressIn, uint8_t dataIn )
{
	return (int32_t)m_pI2C->WriteByte( m_i2cAddress, (uint8_t)addressIn, dataIn );
}

int32_t PCA9539::ReadByte( uint8_t addressIn, uint8_t &dataOut )
{
	return (int32_t)m_pI2C->ReadByte( m_i2cAddress, (uint8_t)addressIn, &dataOut );
}
int32_t PCA9539::ReadNBytes( uint8_t addressIn, uint8_t* dataOut, uint8_t byteCountIn )
{
    return (int32_t)m_pI2C->ReadBytes( m_i2cAddress, (uint8_t)addressIn, dataOut, byteCountIn );
}

