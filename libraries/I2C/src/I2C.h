#pragma once

#include <Arduino.h>

// Architecture specific definitions
#if defined( ARDUINO_ARCH_AVR )
  #include "avr/I2C_Properties.h"
#elif defined( ARDUINO_ARCH_SAMD )
  #include "samd/I2C_Properties.h"
#else
  #error "I2C library used on unsupported architecture!"
#endif

// Shared I2C definitions
#include "I2C_Shared.h"

// Pass this in at build time to enable error counting
#define ENABLE_I2C_DEBUG 1

class I2C
{
public:
  // Attributes
  #ifdef ENABLE_I2C_DEBUG
    i2c::TI2CStats m_statistics;
  #endif

  i2c::EI2CResult m_result;

  // Methods
  I2C(const I2C&) = delete;               // Delete copy constructor
  I2C& operator=(const I2C &) = delete;   // Delete copy assignment operator

  // Arch specific constructors
  #if defined( ARDUINO_ARCH_AVR )
    I2C();                                                          // AVR only has one I2C interface on predetermined pins
  #elif defined( ARDUINO_ARCH_SAMD )
    I2C( SERCOM *sercomInstance, uint8_t pinSDA, uint8_t pinSCL );  // SAMD requires a specific SERCOM interface and pins
    I2C() = delete;                                                 // Delete default constructor
  #endif

  i2c::EI2CResult Enable();
  i2c::EI2CResult Disable();
  i2c::EI2CResult Reset();

  bool LockAddress( uint8_t addressIn );
  void FreeAddress( uint8_t addressIn );

  bool IsAvailable();
  i2c::EI2CResult SetBaudRate( i2c::EI2CBaudRate baudRateIn );

  #ifdef ENABLE_I2C_DEBUG
    uint32_t GetResultCount( i2c::EI2CResult resultIn )
    {
      return m_statistics.GetResultCount( resultIn );
    }
  #endif

  // Write operations
  i2c::EI2CResult WriteByte(           uint8_t slaveAddressIn,                     uint8_t dataIn,                             bool issueRepeatedStart = false );
  i2c::EI2CResult WriteRegisterByte(   uint8_t slaveAddressIn, uint8_t registerIn, uint8_t dataIn,                             bool issueRepeatedStart = false );
  i2c::EI2CResult WriteBytes(          uint8_t slaveAddressIn,                     uint8_t *dataIn,    uint8_t numberBytesIn,  bool issueRepeatedStart = false );
  i2c::EI2CResult WriteRegisterBytes(  uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataIn,    uint8_t numberBytesIn,  bool issueRepeatedStart = false );
  
  // Read operations
  i2c::EI2CResult ReadRegisterByte(   uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut,                           bool issueRepeatedStart = false );
  i2c::EI2CResult ReadRegisterBytes(  uint8_t slaveAddressIn, uint8_t registerIn, uint8_t *dataOut,   uint8_t numberBytesIn,  bool issueRepeatedStart = false );

private:
  // Attributes  
  bool m_isEnabled                      = false;
  i2c::EI2CBaudRate m_baudRate          = i2c::EI2CBaudRate::BAUDRATE_400K;

  uint8_t m_addressLocks[ 127 ]         = { 0 };

  // Custom properties defined for specific architectures
  TI2CProperties m_customProperties;
};

// Arch specific instantiation
#if defined( ARDUINO_ARCH_AVR )
  // AVR
  extern I2C I2C0;

#elif defined( ARDUINO_ARCH_SAMD )
  // SAMD
  #if WIRE_INTERFACES_COUNT > 0
    extern I2C I2C0;
  #endif
  #if WIRE_INTERFACES_COUNT > 1
    extern I2C I2C1;
  #endif
  #if WIRE_INTERFACES_COUNT > 2
    extern I2C I2C2;
  #endif
  #if WIRE_INTERFACES_COUNT > 3
    extern I2C I2C3;
  #endif
  #if WIRE_INTERFACES_COUNT > 4
    extern I2C I2C4;
  #endif
  #if WIRE_INTERFACES_COUNT > 5
    extern I2C I2C5;
  #endif
  
#endif
