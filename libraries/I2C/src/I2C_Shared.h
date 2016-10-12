#pragma once

#include <Arduino.h>

namespace i2c
{
    enum class EI2CBaudRate
    {
        BAUDRATE_100K,
        BAUDRATE_400K
    };

    enum EI2CAction: uint8_t 
    {
        I2C_WRITE   = 0x0,
        I2C_READ	= 0x1
    };

    struct TI2CTransfer
    {
        uint8_t 	slaveAddress;
        uint32_t	length;
        uint8_t		*buffer;

        EI2CAction 	action;
        bool		issueRepeatedStart;
    };

    // I2C Operation Result Codes
    enum EI2CResult : uint8_t
    {
        // Normal results
        RESULT_SUCCESS    	            = 0,    // Operation successful
        RESULT_NACK,                            // Transaction was denied or there was no response

        // Errors
        RESULT_ERR_TIMEOUT,                     // Operation timed out            
        RESULT_ERR_FAILED,                      // Operation failed
        RESULT_ERR_ALREADY_INITIALIZED,         // Interface already initialized
        RESULT_ERR_INVALID_BAUD,                // Invalid baud rate specified
        RESULT_ERR_LOST_ARBITRATION,            // Lost arbitration during transaction
        RESULT_ERR_BAD_ADDRESS,                 // Invalid I2C slave address specified

        // Counter
        RESULT_TYPES
    };

    // Helper struct for collecting error statistics
    struct TI2CStats
    {
        TI2CStats();

        uint32_t results[ EI2CResult::RESULT_TYPES ];

        EI2CResult AddResult( EI2CResult resultIn );

        uint32_t GetResultCount( EI2CResult resultIn );
    };
}