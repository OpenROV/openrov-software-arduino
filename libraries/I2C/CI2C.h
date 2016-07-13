#pragma once

#include "variant.h"
#include "SERCOM.h"
#include "RingBuffer.h"

// Options
// TODO

class CI2C
{
public:
  // Methods
  CI2C( SERCOM *s, uint8_t pinSDA, uint8_t pinSCL );
  virtual ~CI2C();

  I2C::ERetCode Enable( uint32_t baudRateIn, uint16_t optionsIn );
  I2C::ERetCode Disable();
  I2C::ERetCode Scan();

  bool IsAvailable();

  // Write operations
  I2C::ERetCode WriteByte( uint8_t slaveAddressIn, uint8_t dataIn, bool issueRepeatedStart = false );
  I2C::ERetCode WriteByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t dataIn );
  I2C::ERetCode WriteWord( uint8_t slaveAddressIn, uint16_t dataIn );
  I2C::ERetCode WriteWord( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t dataIn );

  I2C::ERetCode WriteBytes( uint8_t slaveAddressIn, uint8_t *dataIn, uint8_t numberBytesIn );
  I2C::ERetCode WriteBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataIn, uint8_t numberBytesIn );
  I2C::ERetCode WriteWords( uint8_t slaveAddressIn, uint16_t *dataIn, uint8_t numberWordsIn );
  I2C::ERetCode WriteWords( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataIn, uint8_t numberWordsIn );

  // Direct read operations (Uses user provided buffer)
  I2C::ERetCode ReadByte( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut );
  I2C::ERetCode ReadWord( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut );

  I2C::ERetCode ReadBytes( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, uint8_t numberBytesIn );
  I2C::ERetCode ReadWords( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut, uint8_t numberWordsIn );

  // Buffered read operations (Uses internal buffer)
  I2C::ERetCode ReadBytes_Buffered( uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut, uint8_t numberBytesIn );
  I2C::ERetCode ReadWords_Buffered( uint8_t slaveAddressIn, uint8_t registerIn, uint16_t *dataOut, uint8_t numberWordsIn );

  uint8_t NextByte();
  uint16_t NextWord();

  uint8_t BytesAvailable();

private:
  // Attributes
  I2C::TTransfer m_transfer                      = {0};
  I2C::TOptions m_options;

  bool m_isEnabled = false;

  static const uint8_t kBufferSize_bytes         = 32;
  static const uint32_t kDefaultBaudRate         = 100000;

  RingBuffer 	m_rxBuffer;                                       // Intermediate buffer that user can request bytes from
  uint8_t     m_pTransferBuffer[ kBufferSize_bytes ] = {0};  // Internal buffer used for single byte reads and writes, and buffered reads
  uint8_t     m_bytesAvailable = 0;

  SERCOM 			*m_pSercom;
  uint8_t 		m_pinSDA;
  uint8_t 		m_pinSCL;
};

#if WIRE_INTERFACES_COUNT > 0
  extern CI2C I2C0;
#endif
#if WIRE_INTERFACES_COUNT > 1
  extern CI2C I2C1;
#endif
#if WIRE_INTERFACES_COUNT > 2
  extern CI2C I2C2;
#endif
#if WIRE_INTERFACES_COUNT > 3
  extern CI2C I2C3;
#endif
#if WIRE_INTERFACES_COUNT > 4
  extern CI2C I2C4;
#endif
#if WIRE_INTERFACES_COUNT > 5
  extern CI2C I2C5;
#endif