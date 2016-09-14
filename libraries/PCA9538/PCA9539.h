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

            enum class PCA9539_REGISTER : uint8_t
            {
                INPUT_PORT = 0x00,
                OUTPUT_PORT = 0x01,
                POLARITY_INVERSION = 0x02,
                CONFIG = 0x03
            };


            //Private member functions


            
            int32_t ReadByte( PCA9539_REGISTER addressIn, uint8_t& dataOut );
            int32_t ReadNBytes( PCA9539_REGISTER addressIn, uint8_t* dataOut, uint8_t byteCountIn );
            int32_t WriteByte( PCA9539_REGISTER addressIn, uint8_t dataIn );


            //Private member attributes
            uint8_t m_i2cAddress;
            CI2C* m_pI2C;

            bool m_isInitialized = false;

    };
}