#pragma once

#include <Arduino.h>
#include <I2C.h>
#include <orutil.h>

namespace ms5803_14ba
{
    // I2C Slave Addresses
    constexpr uint8_t I2C_ADDRESS_A         = 0x76;
    constexpr uint8_t I2C_ADDRESS_B         = 0x77;

    // Commands
    constexpr uint8_t CMD_ADC_READ          = 0x00;
    constexpr uint8_t CMD_RESET             = 0x1E;
    constexpr uint8_t CMD_PROM_READ_BASE    = 0xA0;         // Add coefficient index modifier
    constexpr uint8_t CMD_PRES_CONV_BASE    = 0x40;         // Add OSR modifier
    constexpr uint8_t CMD_TEMP_CONV_BASE    = 0x40 + 0x10;  // Add OSR modifier

    constexpr float kWaterModFresh          = 1.019716f;
    constexpr float kWaterModSalt           = 0.9945f;

    constexpr uint32_t kRetryDelay_ms       = 1000;
    constexpr uint32_t kResetDelay_ms       = 10;

    // Computation constants
    constexpr int32_t POW_2_7  = 128;           // 2^7
    constexpr int32_t POW_2_8  = 256;           // 2^8
    constexpr int32_t POW_2_13 = 8192;          // 2^13
    constexpr int32_t POW_2_15 = 32768;         // 2^15
    constexpr int32_t POW_2_16 = 65536;         // 2^16
    constexpr int32_t POW_2_21 = 2097152;       // 2^21
    constexpr int32_t POW_2_23 = 8388608;       // 2^23
    constexpr int64_t POW_2_33 = 8589934592;    // 2^33
    constexpr int64_t POW_2_37 = 137438953472;  // 2^37

    //typedef void (*TStateCallback)();

    enum class EAddress
    {
        ADDRESS_A,
        ADDRESS_B
    };

    enum class EWaterType
    {
        FRESH   = 0,
        SALT    = 1
    };
    
    enum EState : uint8_t
    {
        UNINITIALIZED,
        DISABLED,
        DELAY,
        READING_CALIB_DATA,
        CONVERTING_PRESSURE,
        CONVERTING_TEMPERATURE,
        PROCESSING_DATA,

        _STATE_COUNT
    };

    enum EResult : uint32_t
    {
        RESULT_SUCCESS,
        
        RESULT_ERR_HARD_RESET,
        RESULT_ERR_FAILED_SEQUENCE,
        RESULT_ERR_I2C_TRANSACTION,
        RESULT_ERR_CRC_MISMATCH,

        _RESULT_COUNT
    };

    // Data structure
    struct TData
    {
        public:
            float temperature_c = 0.0f;
            float pressure_mbar = 0.0f;
            float depth_m       = 0.0f;

            bool SampleAvailable()
            {
                if( isAvailable )
                {
                    isAvailable = false;
                    return true;
                }
                else
                {
                    return false;
                }
            }

        private:
            void Update( float tempIn, float presIn, float waterModIn )
            {
                temperature_c   = tempIn;
                pressure_mbar   = presIn;
                depth_m         = ( presIn - 1013.25f ) * waterModIn / 100.0f;
                isAvailable     = true;
            }

            bool isAvailable = false;

        friend class MS5803_14BA;
    };

    // Delay state info
    struct TTransitionDelay
    {
        orutil::CTimer  timer;
        EState          nextState;
        uint32_t        delayTime_ms;
    };

    // ----------
    // OSR
    enum class EOversampleRate : uint8_t
    {
        OSR_256_SAMPLES = 0,
        OSR_512_SAMPLES,
        OSR_1024_SAMPLES,
        OSR_2048_SAMPLES,
        OSR_4096_SAMPLES,

        _OSR_COUNT
    };

    struct TOversampleInfo
    {
        uint8_t commandMod;
        uint8_t conversionTime_ms;
    };

    // Table of command modifiers and conversion times for each OSR
    constexpr TOversampleInfo kOSRInfo[ (uint8_t)EOversampleRate::_OSR_COUNT ] =
    { 
        { 0x00, 1 },    // 256
        { 0x02, 2 },    // 512
        { 0x04, 3 },    // 1024
        { 0x06, 5 },    // 2048
        { 0x08, 10 }    // 4096
    };
}