#pragma once

#include <Arduino.h>
#include <I2C.h>
#include <orutil.h>

namespace mpu9150
{
    // I2C Slave Addresses
    constexpr uint8_t DEVICE_A         = 0;
    constexpr uint8_t DEVICE_B         = 1;

    enum class EAddress
    {
        ADDRESS_A,
        ADDRESS_B
    };

    enum EResult : uint32_t
    {
		// Successes
        RESULT_SUCCESS,
        
		// Errors
        RESULT_ERR_HARD_RESET,
        RESULT_ERR_READ_ERROR,

		// Counter
        _RESULT_COUNT
    };
}