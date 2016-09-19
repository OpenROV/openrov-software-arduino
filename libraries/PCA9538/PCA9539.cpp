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
    //Set all of the pins as inputs
    m_gpioDirection = 0xFF; 

    //Set all of the pins to the LOW state
    m_gpioState = 0xFF;

    m_isInitialized = true;

    return ERetCode::SUCCESS;
}

//Overloaded function that sets all of the pins to a mode
ERetCode PCA9539::PinMode( uint16_t mode )
{
    if( mode == INPUT )
    {
        //Set all of the pins as inputs
        m_gpioDirection = 0xFF;
    }
    else if( mode == OUTPUT )
    {
        //Set all of the pins as outputs
        m_gpioDirection = 0x00;
    }
    else
    {
        //User described mode. Set it to that
        //TODO: Should we allow this?
        m_gpioDirection = mode;
    }
    
    //Write it
    auto ret = WriteByte( PCA9539_REGISTER::CONFIG, m_gpioDirection);
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_PIN_MODE;
    }
    return ERetCode::SUCCESS;
}

//Set a pin to a mode
ERetCode PCA9539::PinMode( uint8_t pin, bool mode )
{
    //Pins 0..7 are r/w capable pins
    if( pin > 8 )
    {
        return ERetCode::FAILED_PIN_MODE;
    }

    if( mode == INPUT )
    {
        //Set that pin as an input pin (set the bit)
        m_gpioDirection |= ( 1 << pin );

    }
    else if( mode == OUTPUT )
    {
        //Set this pin as an output pin (clear the bit)
        m_gpioDirection &= ~( 1 << pin );
    }
    else
    {
        //Fail out. Don't recognize that pin mode
        return ERetCode::FAILED_PIN_MODE;
    }

    //Write it
    auto ret = WriteByte( PCA9539_REGISTER::CONFIG, m_gpioDirection);
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_PIN_MODE;
    }

    return ERetCode::SUCCESS;
}
    

ERetCode PCA9539::DigitalWrite( uint8_t pin, bool value )
{
    //Pins 0..7 are r/w capable pins
    if( pin > 8 )
    {
        return ERetCode::FAILED_DIGITAL_WRITE;
    }

    if( value == HIGH )
    {
        //Set the bit to high. (set the bit)
        m_gpioState |= ( 1 << pin );
    }
    else if( value == LOW )
    {
        //Set the bit to high. (clear the bit)
        m_gpioState &= ~( 1 << pin );
    }
    else
    {
        //Don't know what this is
        return ERetCode::FAILED_DIGITAL_WRITE;
    }

    //Write it
    auto ret = WriteByte( PCA9539_REGISTER::OUTPUT_PORT, m_gpioState);
    if( ret != I2C::ERetCode::SUCCESS )
    {
        return ERetCode::FAILED_DIGITAL_WRITE;
    }

    return ERetCode::SUCCESS;

}

// value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);

// if(HIGH)
// {
//     value = _gpioState |= (1 << pin);
// }
// else
// {
//     _gpioState &= ~(1 << pin)
// }




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

