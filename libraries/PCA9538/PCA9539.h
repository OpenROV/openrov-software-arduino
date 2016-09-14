/* 
 PCA9539 GPIO Expander Library
 By: OpenROV
 Date: September 14, 2016
*/

#pragma once

//Forward declaration of the I2C interface
class CI2C;

namespace pca9539
{
    //Unshifted 7-bit I2C address for device
    const uint8_t PCA9539_ADDRESS = 0x70; //b1110000

    enum ERetCode : int32_t
    {
        SUCCESS,
        FAILED,
        TIMED_OUT,
        UNKNOWN
    };

    class PCA9539
    {
        public:
            PCA9539( CI2C* i2cInterfaceIn );

            ERetCode Initialize();
            bool IsInitialized() const { return m_isInitialized; };

        private:
            //Private member functions


            
            int32_t ReadByte( uint8_t addressIn, uint8_t& dataOut );
            int32_t ReadNBytes( uint8_t addressIn, uint8_t* dataOut, uint8_t byteCountIn );
            int32_t WriteByte( uint8_t addressIn, uint8_t dataIn );


            //Private member attributes
            uint8_t m_i2cAddress;
            CI2C* m_pI2C;

            bool m_isInitialized = false;

    };
}