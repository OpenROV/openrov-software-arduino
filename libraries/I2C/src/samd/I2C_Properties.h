#pragma once

#include <Arduino.h>
#include <SERCOM.h>

namespace samd
{
    namespace i2c
    {
        constexpr uint8_t kI2CTxBufferSize_bytes           = 32;
        constexpr uint32_t kI2CDefaultTimeout_ms           = 25;

        struct TI2CProperties
        {
            SERCOM 			*m_pSercom;
            uint8_t 		m_pinSDA;
            uint8_t 		m_pinSCL;

            TOptions        m_options;
            TI2CTransfer    m_transfer;
            uint8_t         m_pTransferBuffer[ kI2CTxBufferSize_bytes ];  // Internal buffer used for single byte reads and writes, and buffered reads
            uint32_t        m_timeout_ms = kI2CDefaultTimeout_ms;
        };
    }
}