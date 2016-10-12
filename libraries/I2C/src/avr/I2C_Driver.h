#pragma once

#include <I2C_Shared.h>

namespace i2c
{
    namespace avr
    {
        i2c::EI2CResult Enable( i2c::EI2CBaudRate baudRateIn );
        i2c::EI2CResult Disable();
        i2c::EI2CResult SetBaudRate( i2c::EI2CBaudRate baudRateIn );

        i2c::EI2CResult StartTransaction();
        i2c::EI2CResult SendAddress( uint8_t slaveAddressIn, i2c::EI2CAction actionIn );
        i2c::EI2CResult WriteByte( uint8_t byteIn );
        i2c::EI2CResult ReadByte( uint8_t* dataOut, bool sendNack );
        i2c::EI2CResult StopTransaction();
        i2c::EI2CResult WaitForInterrupt( uint8_t &twiStatusOut );
        i2c::EI2CResult WaitForStop();
    
        void Reset();
    }
}
