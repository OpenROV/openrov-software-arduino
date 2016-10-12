/* 
 PCA9539 GPIO Expander Library
 By: OpenROV
 Date: September 14, 2016
*/

#pragma once

#include <Arduino.h>
#include <I2C.h>

namespace pca9539
{
    //Unshifted 7-bit I2C address for device
    const uint8_t PCA9539_ADDRESS = 0x70; //b1110000

    enum ERetCode : int32_t
    {
        SUCCESS,
        FAILED,
        FAILED_PIN_MODE,
        FAILED_DIGITAL_WRITE,
        TIMED_OUT,
        UNKNOWN
    };

    enum EPin : uint8_t
    {
        LED_0,
        LED_1,
        LED_2,
        LED_3,
        LED_4
    };

    class PCA9539
    {
        public:
            PCA9539( I2C* i2cInterfaceIn );

            ERetCode DigitalWrite( uint8_t pin, bool value );
            ERetCode DigitalWriteHex( uint8_t value );
            ERetCode DigitalWriteDecimal( uint8_t value );

            ERetCode Initialize();
            bool IsInitialized() const { return m_isInitialized; };

            ERetCode PinMode( uint16_t mode );
            ERetCode PinMode( uint8_t pin, bool mode );

        private:

            enum class PCA9539_REGISTER : uint8_t
            {
                INPUT_PORT = 0x00, //Input port registers
                OUTPUT_PORT = 0x01, //Output port registers
                POLARITY_INVERSION = 0x02, //Polarity inversion registers
                CONFIG = 0x03 //Configuration registers
            };


            //Private member functions            
            int32_t ReadByte( PCA9539_REGISTER addressIn, uint8_t& dataOut );
            int32_t ReadNBytes( PCA9539_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn );
            int32_t WriteByte( PCA9539_REGISTER addressIn, uint8_t dataIn );


            //Private member attributes
            uint8_t m_i2cAddress;
            I2C* m_pI2C;

            uint8_t m_gpioDirection;
            uint8_t m_gpioState;

            bool m_isInitialized = false;

    };
}